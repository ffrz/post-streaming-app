#ifndef LOCATIONMANAGER_H
#define LOCATIONMANAGER_H

#include <QDialog>
#include "data.h"

class QSortFilterProxyModel;

namespace Ui {
class LocationManager;
class LocationEditor;
}

class LocationManager : public QDialog
{
    Q_OBJECT

public:
    explicit LocationManager(QWidget *parent = nullptr);
    ~LocationManager();

    Location *selectedItem() const;

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

    Ui::LocationManager *ui;
    Model *model;
    QSortFilterProxyModel *proxyModel;
};

class LocationManager::Editor : public QDialog
{
    Q_OBJECT

public:
    Editor(QWidget *parent);
    ~Editor();

    void edit(Location *loc);

public slots:
    void accept();

private:
    Ui::LocationEditor *ui;
    int id;
};

#endif // LOCATIONMANAGER_H
