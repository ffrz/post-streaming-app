#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"
#include "global.h"
#include "projectdocument.h"

#include <QSettings>
#include <QRegularExpressionValidator>
#include <QDir>
#include <QFileDialog>
#include <QJsonDocument>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

    setWindowTitle(qApp->applicationDisplayName() + " - " + "New Project");
    ui->errorLabel->setVisible(false);
    ui->okButton->setEnabled(false);

    QSettings settings(qApp->property("settings-path").toString(), QSettings::IniFormat);
    ui->locationEdit->setText(settings.value(SK_DEFAULT_PROJECT_LOCATION, QDir::homePath()).toString());

    connect(ui->nameEdit, SIGNAL(textChanged(QString)), SLOT(validateInput()));
    connect(ui->locationEdit, SIGNAL(textChanged(QString)), SLOT(validateInput()));
    connect(ui->okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->browseButton, SIGNAL(clicked()), SLOT(browse()));
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::accept()
{
    validateInput();

    QString projectName = ui->nameEdit->text().trimmed();
    QString dir = ui->locationEdit->text().trimmed();

    _projectDir = dir.append("/").append(projectName);
    _projectPath = QString("%1/%2.%3").arg(_projectDir, projectName, APP_PROJECT_EXTENSION);

    if (ui->defaultLocationCheckbox->isChecked()) {
        QString newDefaultLocation = ui->locationEdit->text();
        QSettings settings(qApp->property("settings-path").toString(), QSettings::IniFormat);
        settings.setValue(SK_DEFAULT_PROJECT_LOCATION, newDefaultLocation);
    }

    QDialog::accept();
}

void NewProjectDialog::validateInput()
{
    QString projectName = ui->nameEdit->text().trimmed();
    QRegularExpression re("^[a-zA-Z0-9_\\-]+$");
    QRegularExpressionMatch match = re.match(projectName);

    QString parentDir = ui->locationEdit->text();
    QString newProjectDir = parentDir + QDir::separator() + projectName;

    bool isValid = false;
    QString errorText = "";

    if (projectName.isEmpty()) {
        errorText = "Silahkan isi nama project!";
    }
    else if (!match.hasMatch()) {
        errorText = "Nama project tidak valid, gunakan alfabet, angka atau _ (underscore).";
    }
    else if (!QDir(parentDir).exists()) {
        errorText = "Lokasi tidak valid.";
    }
    else if (QDir(newProjectDir).exists()) {
        errorText = "Direktori project sudah ada, gunakan nama lain.";
    }
    else {
        isValid = true;
    }

    ui->errorLabel->setText(errorText);
    ui->errorLabel->setVisible(!isValid);
    ui->okButton->setEnabled(isValid);
}

void NewProjectDialog::browse()
{
    QString currentDir = ui->locationEdit->text();
    if (!QDir(currentDir).exists())
        currentDir = QString();

    QString directory = QFileDialog::getExistingDirectory(this, "Pilih direktori", currentDir);
    if (directory.isEmpty())
        return;

    if (!QDir(directory).exists())
        return;

    ui->locationEdit->setText(directory);
}
