#ifndef STUDYTEXTTEMPLATEMANAGER_H
#define STUDYTEXTTEMPLATEMANAGER_H

#include <QDialog>
#include "data.h"

class QSortFilterProxyModel;

namespace Ui {
class StudyTextTemplateManager;
class StudyTextTemplateEditor;
}

class StudyTextTemplateManager : public QDialog
{
    Q_OBJECT

public:
    explicit StudyTextTemplateManager(QWidget *parent = nullptr);
    ~StudyTextTemplateManager();
    StudyTextTemplate *selectedItem() const;

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

    Ui::StudyTextTemplateManager *ui;
    Model *model;
    QSortFilterProxyModel *proxyModel;
};

class StudyTextTemplateManager::Editor : public QDialog
{
    Q_OBJECT

public:
    Editor(QWidget *parent);
    ~Editor();
    void edit(StudyTextTemplate *data);

public slots:
    void accept();

private slots:
    void onInsertComboBoxActivated(int);

private:
    Ui::StudyTextTemplateEditor *ui;
    int id;
};

#endif // STUDYTEXTTEMPLATEMANAGER_H
