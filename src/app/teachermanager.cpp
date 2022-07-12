#include "teachermanager.h"
#include "ui_teachermanager.h"
#include "ui_teachereditor.h"
#include "db.h"

#include <QSortFilterProxyModel>
#include <QMessageBox>

class TeacherManager::Model : public QAbstractTableModel
{
public:
    typedef Teacher Item;
    QList<Item> items;

    Model(QObject *parent) : QAbstractTableModel(parent) {}

    void refresh()
    {
        beginResetModel();
        items = db::getAllTeachersOrderById();
        endResetModel();
    }

    QVariant headerData(int section, Qt::Orientation o, int role) const
    {
        if (o == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
            case 0: return "Nama Ustadz";
            case 1: return "Jabatan";
            }
        }
        return QVariant();
    }

    int rowCount(const QModelIndex & = QModelIndex()) const
    {
        return items.size();
    }

    int columnCount(const QModelIndex & = QModelIndex()) const
    {
        return 3;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (index.isValid()) {
            Item item = items.at(index.row());
            if (role == Qt::DisplayRole) {
                switch (index.column()) {
                case 0: return item.name;
                case 1: return item.description;
                }
            }
        }
        return QVariant();
    }
};

TeacherManager::TeacherManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TeacherManager),
    model(new Model(this)),
    proxyModel(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);

    proxyModel->setSourceModel(model);
    ui->tableView->setModel(proxyModel);

    connect(ui->addButton, SIGNAL(clicked()), SLOT(add()));
    connect(ui->removeButton, SIGNAL(clicked()), SLOT(remove()));
    connect(ui->tableView, SIGNAL(activated(QModelIndex)), SLOT(edit()));

    refresh();
}

TeacherManager::Editor::Editor(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::TeacherEditor)
    , id(0)
{
    ui->setupUi(this);

    connect(ui->saveButton, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
}

TeacherManager::Editor::~Editor()
{
    delete ui;
}

void TeacherManager::Editor::edit(Teacher *item)
{
    id = item->id;
    ui->nameEdit->setText(item->name);
    ui->descriptionEdit->setText(item->description);
}

void TeacherManager::Editor::accept()
{
    Teacher item;

    item.id = id;
    item.name = ui->nameEdit->text().trimmed();
    item.description = ui->descriptionEdit->text().trimmed();

    if (item.name.isEmpty()) {
        QMessageBox::warning(this, "Peringatan", "Nama ustadz harus diisi.");
        ui->nameEdit->setFocus();
        return;
    }

    db::saveTeacher(item);

    QDialog::accept();
}

TeacherManager::~TeacherManager()
{
    delete ui;
}

Teacher *TeacherManager::selectedItem() const
{
    QModelIndex current = ui->tableView->currentIndex();
    if (!current.isValid())
        return nullptr;

    return &model->items[proxyModel->mapToSource(current).row()];
}

void TeacherManager::add()
{
    Editor editor(this);
    editor.setWindowTitle("Tambah Ustadz");
    if (!editor.exec())
        return;

    refresh();

    emit dataChanged();
}

void TeacherManager::edit()
{
    Model::Item *item = selectedItem();
    if (!item)
        return;

    QString nameTruncated = item->name;
    nameTruncated.truncate(20);

    Editor editor(this);
    editor.setWindowTitle(QString("Edit %1..").arg(nameTruncated));
    editor.edit(item);
    if (!editor.exec())
        return;

    refresh();

    emit dataChanged();
}

void TeacherManager::remove()
{
    Model::Item *item = selectedItem();
    if (!item)
        return;

    if (QMessageBox::question(this, "Konfirmasi", "Hapus rekaman yang dipilih?") != QMessageBox::Yes)
        return;

    db::removeTeacher(*item);

    refresh();

    emit dataChanged();
}

void TeacherManager::refresh()
{
    model->refresh();
    ui->infoLabel->setText(QString("Menampilkan %1 rekaman.")
                           .arg(locale().toString(model->rowCount())));
}
