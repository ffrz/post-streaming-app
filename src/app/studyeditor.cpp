#include "studyeditor.h"
#include "ui_studyeditor.h"
#include "projectdocument.h"
#include "db.h"
#include <QTimer>
#include <QSqlQuery>
#include <QFileDialog>
#include <QProcess>
#include <QThread>
#include <QMessageBox>

class AudioCompressorThread : public QThread
{
public:
    QString command;
    AudioCompressorThread(QObject* parent)
        : QThread(parent) { }

    void run()
    {
        QProcess ffmpegProcess;
        ffmpegProcess.startCommand(command);
        ffmpegProcess.waitForFinished(-1);
    }
};

StudyEditor::StudyEditor(ProjectDocument *d, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudyEditor)
    , doc(d)
    , audioProcess(new AudioCompressorThread(this))
    , progressTimer(new QTimer(this))
    , _estimatedOutputAudioSize(0.)
{
    ui->setupUi(this);

    ui->audioCompressionComboBox->addItem("8 kb/s", "8k");
    ui->audioCompressionComboBox->addItem("16 kb/s", "16k");
    ui->audioCompressionComboBox->addItem("24 kb/s", "24k");
    ui->audioCompressionComboBox->addItem("32 kb/s", "32k");
    ui->audioCompressionComboBox->addItem("40 kb/s", "40k");
    ui->audioCompressionComboBox->addItem("48 kb/s", "48k");
    ui->audioCompressionComboBox->addItem("64 kb/s", "64k");
    ui->audioCompressionComboBox->setCurrentIndex(3);

    progressTimer->setInterval(500);

    ui->dateEdit->setMinimumDate(QDate(2021, 8, 10)); // 1 Muharram 1443
    ui->dateEdit->setDate(QDate::currentDate());

    refreshTeachers();
    refreshLocations();
    refreshBooks();

    ui->teacherComboBox->setCurrentIndex(-1);
    ui->studyTitleComboBox->setCurrentIndex(-1);
    ui->placeComboBox->setCurrentIndex(-1);

    // inisisalisasi form dari project
    QJsonObject data = doc->data();
    ui->studyTypeComboBox->setCurrentIndex(data.value(ProjectDocument::DATA_KEY_STUDY_TYPE).toInt());
    ui->studyTitleComboBox->lineEdit()->setText(data.value(ProjectDocument::DATA_KEY_TITLE).toString());
    ui->studySubTitleEdit->setText(data.value(ProjectDocument::DATA_KEY_SUBTITLE).toString());
    ui->authorEdit->setText(data.value(ProjectDocument::DATA_KEY_AUTHOR).toString());
    ui->narratorEdit->setText(data.value(ProjectDocument::DATA_KEY_NARRATOR).toString());
    ui->studyDescriptionEdit->setPlainText(data.value(ProjectDocument::DATA_KEY_STUDY_DESCRIPTION).toString());
    ui->teacherComboBox->lineEdit()->setText(data.value(ProjectDocument::DATA_KEY_TEACHER).toString());
    ui->teacherDescriptionEdit->setText(data.value(ProjectDocument::DATA_KEY_TEACHER_DETAILS).toString());
    ui->studyUrlEdit->setText(data.value(ProjectDocument::DATA_KEY_STUDY_URL).toString());
    ui->dateEdit->setDate(QDate::fromString(data.value(ProjectDocument::DATA_KEY_DATE).toString(), "yyyy-MM-dd"));
    QString hijriDate = data.value(ProjectDocument::DATA_KEY_HIJRI_DATE).toString();
    QStringList hijriDateStringList = hijriDate.split("-");
    if (hijriDateStringList.size() == 3) {
        QStringList hijriDateStringList = hijriDate.split("-");
        ui->hijriYearSpinBox->setValue(hijriDateStringList.at(0).toInt());
        ui->hijriMonthComboBox->setCurrentIndex(hijriDateStringList.at(1).toInt() - 1);
        ui->hijriDateSpinBox->setValue(hijriDateStringList.at(2).toInt());
    }
    ui->startTimeComboBox->lineEdit()->setText(data.value(ProjectDocument::DATA_KEY_START_TIME).toString());
    ui->endTimeComboBox->lineEdit()->setText(data.value(ProjectDocument::DATA_KEY_END_TIME).toString());
    ui->placeComboBox->lineEdit()->setText(data.value(ProjectDocument::DATA_KEY_LOCATION_NAME).toString());
    ui->addressEdit->setText(data.value(ProjectDocument::DATA_KEY_LOCATION_ADDRESS).toString());
    ui->locationUrlEdit->setText(data.value(ProjectDocument::DATA_KEY_LOCATION_URL).toString());

    // connect signal slots
    connect(ui->audioEdit, SIGNAL(textChanged(QString)), SLOT(recalculateEstimatedOutputAudioSize()));
    connect(ui->dateEdit, SIGNAL(dateChanged(QDate)), SLOT(onDateChanged(QDate)));
    connect(ui->teacherComboBox, SIGNAL(activated(int)), SLOT(onTeacherChanged(int)));
    connect(ui->placeComboBox, SIGNAL(activated(int)), SLOT(onPlaceChanged(int)));
    connect(ui->studyTitleComboBox, SIGNAL(activated(int)), SLOT(onStudyTitleChanged(int)));
    connect(ui->studyTypeComboBox, SIGNAL(activated(int)), SLOT(onStudyTypeChanged(int)));
    connect(ui->browseAudioButton, SIGNAL(clicked()), SLOT(onBrowseAudioButtonClicked()));
    connect(ui->browseStudyTextTemplateButton, SIGNAL(clicked()), SLOT(onBrowseStudyTextTemplateButton()));
    connect(ui->browseStudyAudioTextTemplateButton, SIGNAL(clicked()), SLOT(onBrowseStudyAudioTextTemplateButton()));
    connect(ui->audioCompressionComboBox, SIGNAL(currentIndexChanged(int)), SLOT(recalculateEstimatedOutputAudioSize()));
    connect(ui->processButton, SIGNAL(clicked()), SLOT(onProcessButtonClicked()));
    connect(audioProcess, SIGNAL(finished()), SLOT(onAudioProcessFinished()));
    connect(progressTimer, SIGNAL(timeout()), SLOT(checkAudioProgress()));

    // final initialization
    onStudyTypeChanged(ui->studyTypeComboBox->currentIndex());
}

StudyEditor::~StudyEditor()
{
    delete doc;
    delete ui;
}

void StudyEditor::onTeacherChanged(int index)
{
    if (index == -1) {
        ui->teacherDescriptionEdit->clear();
        return;
    }

    Teacher teacher = db::getTeacherById(ui->teacherComboBox->currentData().toInt());
    ui->teacherDescriptionEdit->setText(teacher.description);
}

void StudyEditor::onPlaceChanged(int index)
{
    if (index == -1) {
        ui->addressEdit->clear();
        ui->locationUrlEdit->clear();
        return;
    }

    Location location = db::getLocationById(ui->placeComboBox->currentData().toInt());
    ui->addressEdit->setText(location.address);
    ui->locationUrlEdit->setText(location.url);
}

void StudyEditor::onStudyTitleChanged(int index)
{
    if (index == -1) {
        ui->studySubTitleEdit->clear();
        ui->authorEdit->clear();
        ui->narratorEdit->clear();
        return;
    }

    Book book = db::getBookById(ui->studyTitleComboBox->currentData().toInt());
    ui->studySubTitleEdit->setText(book.subtitle);
    ui->authorEdit->setText(book.authors);
    ui->narratorEdit->setText(book.narrator);
}

void StudyEditor::onBrowseAudioButtonClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Pilih Audio Kajian", "", "*.mp3");

    if (filename.isEmpty()) {
        ui->audioEdit->clear();
        return;
    }

    ui->audioEdit->setText(filename);

    recalculateEstimatedOutputAudioSize();
}

void StudyEditor::onBrowseStudyTextTemplateButton()
{
    QString filename = QFileDialog::getOpenFileName(this, "Pilih Template Teks Kajian", "", "*.txt");

    if (filename.isEmpty()) {
        ui->studyTextTemplateEdit->clear();
        return;
    }

    ui->studyTextTemplateEdit->setText(filename);
}

void StudyEditor::onBrowseStudyAudioTextTemplateButton()
{
    QString filename = QFileDialog::getOpenFileName(this, "Pilih Template Audio Teks Kajian", "", "*.txt");

    if (filename.isEmpty()) {
        ui->studyAudioTextTemplateEdit->clear();
        return;
    }

    ui->studyAudioTextTemplateEdit->setText(filename);
}

void StudyEditor::onDateChanged(const QDate& date)
{
    QDate firstDay(2021, 8, 10); // 1 Muharram 1443

    int year = 1443;
    double day = firstDay.daysTo(date);
    int month = 1;

    while (day > 354) {
        day = day - 354;
        year++;
    }

    while (day > 29.5) {
       day -= 29.5;
       month++;
    }

    ui->hijriDateSpinBox->setValue(ceil(day));
    ui->hijriMonthComboBox->setCurrentIndex(month - 1);
    ui->hijriYearSpinBox->setValue(year);
}

void StudyEditor::onProcessButtonClicked()
{
    QString studyTitle = ui->studyTitleComboBox->lineEdit()->text().trimmed();;
    //QString studySubTitle = ui->studySubTitleEdit->text().trimmed();;
    if (studyTitle.isEmpty()) {
        ui->studyTitleComboBox->setFocus();
        return;
    }
    QString teacherName = ui->teacherComboBox->lineEdit()->text().trimmed();
    //QString teacherDescription = ui->teacherDescriptionEdit->text().trimmed();
    QString studyType = ui->studyTypeComboBox->currentText();

    // generate teks

    QString audioInputFileName = ui->audioEdit->text().trimmed();
    if (audioInputFileName.isEmpty()) {
        // tidak perlu eksekusi ffmpeg
        return;
    }

    QFileInfo docFile(doc->fileName());
    QDir projectDir = docFile.absoluteDir();
    if (!projectDir.exists("output")) {
        projectDir.mkdir("output");
    }
    QString outputDir = projectDir.absolutePath() + QDir::separator() +"output";

    _inputAudioFileName = audioInputFileName;
    _outputAudioFileName = outputDir + QDir::separator() + QString("Al Ustadz " + teacherName + " - " + studyTitle + ".mp3");;
    QString compression = ui->audioCompressionComboBox->currentData().toString();
    // generate teks audio
    // compress & update mp3 tag
    audioProcess->command =
            QString("ffmpeg -y -i \"%1\""
                    " -metadata title=\"%2\" -metadata artist=\"%3\" -metadata album=\"%4\""
                    " -map 0:a:0 -b:a %5 \"%6\"")
            .arg(audioInputFileName, studyTitle, teacherName, studyType, compression,
                 _outputAudioFileName);

    audioProcess->start();
    ui->processButton->setEnabled(false);
    ui->audioProcessProgressBar->setEnabled(true);
    ui->audioProcessProgressBar->setValue(0);
    progressTimer->start();
}

void StudyEditor::onStudyTypeChanged(int index)
{
    bool isBookStudy = index == 1;
    ui->authorEdit->setEnabled(isBookStudy);
    ui->narratorEdit->setEnabled(isBookStudy);
    ui->studyDescriptionEdit->setEnabled(isBookStudy);
}

void StudyEditor::save()
{
    QJsonObject data;
    data.insert(ProjectDocument::DATA_KEY_STUDY_TYPE, ui->studyTypeComboBox->currentIndex());
    data.insert(ProjectDocument::DATA_KEY_TITLE, ui->studyTitleComboBox->lineEdit()->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_SUBTITLE, ui->studySubTitleEdit->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_AUTHOR, ui->authorEdit->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_NARRATOR, ui->narratorEdit->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_STUDY_DESCRIPTION, ui->studyDescriptionEdit->toPlainText());
    data.insert(ProjectDocument::DATA_KEY_TEACHER, ui->teacherComboBox->lineEdit()->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_TEACHER_DETAILS, ui->teacherDescriptionEdit->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_STUDY_URL, ui->studyUrlEdit->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_DATE, ui->dateEdit->date().toString("yyyy-MM-dd"));
    data.insert(ProjectDocument::DATA_KEY_HIJRI_DATE, QString("%1-%2-%3").arg(
                    QString::number(ui->hijriYearSpinBox->value()),
                    QString::number(ui->hijriMonthComboBox->currentIndex() + 1),
                    QString::number(ui->hijriDateSpinBox->value())));
    data.insert(ProjectDocument::DATA_KEY_START_TIME, ui->startTimeComboBox->lineEdit()->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_END_TIME, ui->endTimeComboBox->lineEdit()->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_LOCATION_NAME, ui->placeComboBox->lineEdit()->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_LOCATION_ADDRESS, ui->addressEdit->text().trimmed());
    data.insert(ProjectDocument::DATA_KEY_LOCATION_URL, ui->locationUrlEdit->text().trimmed());

    doc->setData(data);
    doc->save();
}

void StudyEditor::onAudioProcessFinished()
{
    ui->audioProcessProgressBar->setValue(100);
    progressTimer->stop();
    ui->processButton->setEnabled(true);
    ui->audioProcessProgressBar->setEnabled(false);
    QMessageBox::information(this, "Informasi", "Kompresi audio telah selesai.");
    ui->audioProcessProgressBar->reset();
}

void StudyEditor::checkAudioProgress()
{
    QFileInfo inputFile(_inputAudioFileName);
    QFileInfo outputFile(_outputAudioFileName);

    if (!(inputFile.exists() && outputFile.exists())) {
        qDebug() << "invalid input info or output file info.";
        return;
    }

    double outSize = outputFile.size();

    int percent = 0;
    if (outSize > 0 && _estimatedOutputAudioSize > 0)
        percent = (outSize / _estimatedOutputAudioSize) * 100.;
    ui->audioProcessProgressBar->setValue((int)percent);
}

void StudyEditor::recalculateEstimatedOutputAudioSize()
{
    ui->processButton->setEnabled(false);
    ui->ouputFileSizeLabel->setStyleSheet("");

    QString filename = ui->audioEdit->text().trimmed();
    if (!QFile::exists(filename)) {
        ui->ouputFileSizeLabel->setText("File audio tidak ditemukan.");
        ui->ouputFileSizeLabel->setStyleSheet("color: red");
        return;
    }

    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.setProgram("ffprobe");
    p.setArguments(QStringList() << filename);
    p.start();
    p.waitForFinished();
    QString data = p.readAllStandardOutput();

    QRegularExpressionMatch rxm = QRegularExpression("(Duration: \\d+:\\d+:\\d+.\\d+)").match(data);
    QString s = rxm.captured(1);
    s = s.replace("Duration: ", "");
    QStringList d = s.split(":");
    if (d.size() == 3) {
        double duration = (d[0].toInt() * 60 * 60) + (d[1].toInt() * 60) + d[2].toDouble();
        QString bitRateText = ui->audioCompressionComboBox->currentData().toString();
        _estimatedOutputAudioSize = (duration * (bitRateText.replace("k", "000").toDouble() / 8.));
        ui->ouputFileSizeLabel->setText(QString("%1 MB (%2 bytes)")
                                        .arg(locale().toString(_estimatedOutputAudioSize / 1000000., 'f', 2),
                                             locale().toString(_estimatedOutputAudioSize, 'f', 0)));
    }
    ui->processButton->setEnabled(true);
}

void StudyEditor::refreshTeachers()
{
    QString oldText = ui->teacherComboBox->lineEdit()->text();
    ui->teacherComboBox->clear();
    for (const Teacher &d: db::getAllTeachersOrderByName()) {
        ui->teacherComboBox->addItem(d.name, d.id);
    }
    ui->teacherComboBox->lineEdit()->setText(oldText);
}

void StudyEditor::refreshLocations()
{
    QString oldText = ui->placeComboBox->lineEdit()->text();
    ui->placeComboBox->clear();
    for (const Location &d: db::getAllLocationsOrderByName()) {
        ui->placeComboBox->addItem(d.name, d.id);
    }
    ui->placeComboBox->lineEdit()->setText(oldText);
}

void StudyEditor::refreshBooks()
{
    QString oldText = ui->studyTitleComboBox->lineEdit()->text();
    ui->studyTitleComboBox->clear();
    for (const Book &d: db::getAllBooksOrderByTitle()) {
        ui->studyTitleComboBox->addItem(d.title, d.id);
    }
    ui->studyTitleComboBox->lineEdit()->setText(oldText);
}
