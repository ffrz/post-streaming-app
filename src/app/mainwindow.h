#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class StudyEditor;
class ProjectDocument;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void newProject();
    void openProject();
    bool closeProject();
    void saveProject();

    void exit();
    void about();

    void manageTeachers();
    void manageBooks();
    void manageLocations();
    void manageStudyTextTemplates();

private slots:
    void updateMenu();

private:
    void openEditor(ProjectDocument *doc);

    Ui::MainWindow *ui;
    StudyEditor *editor;
};

#endif // MAINWINDOW_H
