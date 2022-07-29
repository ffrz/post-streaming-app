#ifndef STUDYEDITOR_H
#define STUDYEDITOR_H

#include <QWidget>

class ProjectDocument;

QT_BEGIN_NAMESPACE
namespace Ui { class StudyEditor; }
QT_END_NAMESPACE

class AudioCompressorThread;

class StudyEditor : public QWidget
{
    Q_OBJECT

public:
    StudyEditor(ProjectDocument *doc, QWidget *parent = nullptr);
    ~StudyEditor();
    void save();

public slots:
    void refreshTeachers();
    void refreshLocations();
    void refreshBooks();
    void refreshStudyTextTemplates();

private slots:
    void onTeacherChanged(int);
    void onPlaceChanged(int);
    void onStudyTypeChanged(int);
    void onStudyTitleChanged(int);
    void onBrowseAudioButtonClicked();
    void onDateChanged(const QDate&);
    void onProcessButtonClicked();
    void onAudioProcessFinished();
    void checkAudioProgress();
    void recalculateEstimatedOutputAudioSize();
    void showRreviewStudyTextDialog();

private:
    Ui::StudyEditor *ui;
    ProjectDocument *doc;
    AudioCompressorThread *audioProcess;
    QTimer *progressTimer;
    QString _inputAudioFileName;
    QString _outputAudioFileName;
    double _estimatedOutputAudioSize;
};
#endif // STUDYEDITOR_H
