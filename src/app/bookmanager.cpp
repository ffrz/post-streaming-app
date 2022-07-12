#include "bookmanager.h"
#include "ui_bookmanager.h"
#include "ui_bookeditor.h"
#include "db.h"

#include <QSortFilterProxyModel>
#include <QMessageBox>

class BookManager::Model : public QAbstractTableModel
{
public:
    typedef Book Item;
    QList<Item> items;

    Model(QObject *parent) : QAbstractTableModel(parent) {}

    void refresh()
    {
        beginResetModel();
        items = db::getAllBooksOrderById();
        endResetModel();
    }

    QVariant headerData(int section, Qt::Orientation o, int role) const
    {
        if (o == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
            case 0: return "Judul";
            case 1: return "Sub Judul";
            case 2: return "Pengarang";
            case 3: return "Pensyarah";
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
        return 5;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (index.isValid()) {
            Item item = items.at(index.row());
            if (role == Qt::DisplayRole) {
                switch (index.column()) {
                case 0: return item.title;
                case 1: return item.subtitle;
                case 2: return item.authors;
                case 3: return item.narrator;
                }
            }
        }
        return QVariant();
    }
};

BookManager::Editor::Editor(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::BookEditor)
    , id(0)
{
    ui->setupUi(this);

    connect(ui->saveButton, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
}

BookManager::Editor::~Editor()
{
    delete ui;
}

void BookManager::Editor::edit(Book *item)
{
    id = item->id;
    ui->titleEdit->setText(item->title);
    ui->subtitleEdit->setText(item->subtitle);
    ui->authorsEdit->setText(item->authors);
    ui->narratorEdit->setText(item->narrator);
}

void BookManager::Editor::accept()
{
    Book item;

    item.id = id;
    item.title = ui->titleEdit->text().trimmed();
    item.subtitle= ui->subtitleEdit->text().trimmed();
    item.authors = ui->authorsEdit->text().trimmed();
    item.narrator = ui->narratorEdit->text().trimmed();

    if (item.title.isEmpty()) {
        QMessageBox::warning(this, "Peringatan", "Nama kitab harus diisi.");
        ui->titleEdit->setFocus();
        return;
    }

    db::saveBook(item);

    QDialog::accept();
}

BookManager::BookManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookManager),
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

BookManager::~BookManager()
{
    delete ui;
}

Book *BookManager::selectedItem() const
{
    QModelIndex current = ui->tableView->currentIndex();
    if (!current.isValid())
        return nullptr;

    return &model->items[proxyModel->mapToSource(current).row()];
}

void BookManager::add()
{
    Editor editor(this);
    editor.setWindowTitle("Tambah Kitab");
    if (!editor.exec())
        return;

    emit dataChanged();

    refresh();
}

void BookManager::edit()
{
    Model::Item *item = selectedItem();
    if (!item)
        return;

    QString title = item->title;
    title.truncate(20);

    Editor editor(this);
    editor.setWindowTitle(QString("Edit %1..").arg(title));
    editor.edit(item);

    if (!editor.exec())
        return;

    emit dataChanged();

    refresh();
}

void BookManager::remove()
{
    Model::Item *item = selectedItem();
    if (!item)
        return;

    if (QMessageBox::question(this, "Konfirmasi", "Hapus rekaman yang dipilih?") != QMessageBox::Yes)
        return;

    db::removeBook(*item);

    emit dataChanged();

    refresh();
}

void BookManager::refresh()
{
    model->refresh();
    ui->infoLabel->setText(QString("Menampilkan %1 rekaman.")
                           .arg(locale().toString(model->rowCount())));
}
