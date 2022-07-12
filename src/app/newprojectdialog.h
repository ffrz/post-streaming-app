#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = nullptr);
    ~NewProjectDialog();

    inline QString projectDir() const { return _projectDir; }
    inline QString projectPath() const { return _projectPath; }

public slots:
    void validateInput();
    void browse();
    void accept();

private:
    Ui::NewProjectDialog *ui;
    QString _projectDir;
    QString _projectPath;
};

#endif // NEWPROJECTDIALOG_H
