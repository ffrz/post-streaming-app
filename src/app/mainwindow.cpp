#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include "aboutdialog.h"
#include "studyeditor.h"
#include "newprojectdialog.h"
#include "projectdocument.h"
#include "locationmanager.h"
#include "bookmanager.h"
#include "teachermanager.h"

#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    editor(nullptr)
{
    ui->setupUi(this);

    setWindowTitle(qApp->applicationDisplayName());

    connect(ui->newAction, SIGNAL(triggered(bool)), SLOT(newProject()));
    connect(ui->openAction, SIGNAL(triggered(bool)), SLOT(openProject()));
    connect(ui->closeAction, SIGNAL(triggered(bool)), SLOT(closeProject()));
    connect(ui->saveAction, SIGNAL(triggered(bool)), SLOT(saveProject()));
    connect(ui->exitAction, SIGNAL(triggered(bool)), SLOT(exit()));
    connect(ui->aboutAction, SIGNAL(triggered(bool)), SLOT(about()));

    connect(ui->manageTeachersAction, SIGNAL(triggered(bool)), SLOT(manageTeachers()));
    connect(ui->manageBooksAction, SIGNAL(triggered(bool)), SLOT(manageBooks()));
    connect(ui->manageLocationsAction, SIGNAL(triggered(bool)), SLOT(manageLocations()));

    updateMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMenu()
{
    bool hasActiveProject = editor != nullptr;
    ui->saveAction->setEnabled(hasActiveProject);
    ui->saveAsAction->setEnabled(hasActiveProject);
    ui->closeAction->setEnabled(hasActiveProject);
}

void MainWindow::openEditor(ProjectDocument *doc)
{
    editor = new StudyEditor(doc, this);
    setCentralWidget(editor);
    updateMenu();

    QFileInfo fileInfo(doc->fileName());
    setWindowTitle(QString("%1 - %2").arg(qApp->applicationDisplayName(), fileInfo.fileName()));
}

bool MainWindow::closeProject()
{
    if (editor == nullptr)
        return true;

    if (QMessageBox::question(
                this, "Konfirmasi", "Anda yakin akan menutup project yang sedang aktif?")
                == QMessageBox::No)
        return false;

    editor->close();
    editor->deleteLater();
    editor = nullptr;

    updateMenu();
    setWindowTitle(qApp->applicationDisplayName());

    return true;
}

void MainWindow::newProject()
{
    if (!closeProject())
        return;

    NewProjectDialog dialog(this);

    if (!dialog.exec())
        return;

    QDir().mkdir(dialog.projectDir());
    QString fileName = dialog.projectPath();

    // editor bertanggungjawab menghapus doc di destructor
    ProjectDocument *doc = new ProjectDocument(fileName);
    doc->initialize();
    doc->save();

    openEditor(doc);
    updateMenu();
}

void MainWindow::openProject()
{
    if (!closeProject())
        return;

    QSettings settings(qApp->property("settings-path").toString(), QSettings::IniFormat);
    QString dir = settings.value(SK_DEFAULT_PROJECT_LOCATION, QDir::homePath()).toString();
    QString fileName = QFileDialog::getOpenFileName(this, "Buka Project", dir,
                                 "Project Kajian (*." APP_PROJECT_EXTENSION ")", nullptr);

    if (fileName.isEmpty() || !QFile::exists(fileName))
        return;

    openEditor(ProjectDocument::open(fileName));
    updateMenu();
}

void MainWindow::saveProject()
{
    if (editor) {
        editor->save();
    }
}

void MainWindow::exit()
{
    closeProject();
    qApp->quit();
}

void MainWindow::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::manageTeachers()
{
    TeacherManager manager(this);
    if (editor) {
        connect(&manager, SIGNAL(dataChanged()), editor, SLOT(refreshTeachers()));
    }
    manager.exec();
}

void MainWindow::manageBooks()
{
    BookManager manager(this);
    if (editor) {
        connect(&manager, SIGNAL(dataChanged()), editor, SLOT(refreshBooks()));
    }
    manager.exec();
}

void MainWindow::manageLocations()
{
    LocationManager manager(this);
    if (editor) {
        connect(&manager, SIGNAL(dataChanged()), editor, SLOT(refreshLocations()));
    }
    manager.exec();
}
