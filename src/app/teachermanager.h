#ifndef TEACHERMANAGER_H
#define TEACHERMANAGER_H

#include <QDialog>
#include "data.h"

class QSortFilterProxyModel;

namespace Ui {
class TeacherManager;
class TeacherEditor;
}

class TeacherManager : public QDialog
{
    Q_OBJECT

public:
    explicit TeacherManager(QWidget *parent = nullptr);
    ~TeacherManager();
    Teacher *selectedItem() const;

signals:
    void dataChanged();

public slots:
    void add();
    void edit();
    void remove();
    void refresh();

private:
    class Editor;
    class Model;

    Ui::TeacherManager *ui;
    Model *model;
    QSortFilterProxyModel *proxyModel;
};

class TeacherManager::Editor : public QDialog
{
    Q_OBJECT

public:
    Editor(QWidget *parent);
    ~Editor();
    void edit(Teacher *loc);

public slots:
    void accept();

private:
    Ui::TeacherEditor *ui;
    int id;
};

#endif // TEACHERMANAGER_H
