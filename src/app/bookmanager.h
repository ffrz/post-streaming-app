#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H

#include <QDialog>
#include "data.h"

class QSortFilterProxyModel;

namespace Ui {
class BookManager;
class BookEditor;
}

class BookManager : public QDialog
{
    Q_OBJECT

public:
    explicit BookManager(QWidget *parent = nullptr);
    ~BookManager();

    Book *selectedItem() const;

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

    Ui::BookManager *ui;
    Model *model;
    QSortFilterProxyModel *proxyModel;
};

class BookManager::Editor : public QDialog
{
    Q_OBJECT

public:
    Editor(QWidget *parent);
    ~Editor();

    void edit(Book *item);

public slots:
    void accept();

private:
    Ui::BookEditor *ui;
    int id;
};

#endif // BOOKMANAGER_H
