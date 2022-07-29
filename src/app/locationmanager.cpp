#include "locationmanager.h"
#include "ui_locationmanager.h"
#include "ui_locationeditor.h"
#include "db.h"
#include <QSortFilterProxyModel>
#include <QMessageBox>

class LocationManager::Model : public QAbstractTableModel
{
public:
    typedef Location Item;
    QList<Item> items;

    Model(QObject *parent) : QAbstractTableModel(parent) {}

    void refresh()
    {
        beginResetModel();
        items = db::getAllLocationsOrderById();
        endResetModel();
    }

    QVariant headerData(int section, Qt::Orientation o, int role) const
    {
        if (o == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
            case 0: return "Nama Tempat";
            case 1: return "Alamat";
            case 2: return "URL";
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
        return 4;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (index.isValid()) {
            Item item = items.at(index.row());
            if (role == Qt::DisplayRole) {
                switch (index.column()) {
                case 0: return item.name;
                case 1: return item.address;
                case 2: return item.url;
                }
            }
        }
        return QVariant();
    }
};

LocationManager::Editor::Editor(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LocationEditor)
    , id(0)
{
    ui->setupUi(this);

    connect(ui->saveButton, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
}

LocationManager::Editor::~Editor()
{
    delete ui;
}

void LocationManager::Editor::edit(Location *item)
{
    id = item->id;
    ui->nameEdit->setText(item->name);
    ui->addressEdit->setPlainText(item->address);
    ui->urlEdit->setText(item->url);
}

void LocationManager::Editor::accept()
{
    Location item;

    item.id = id;
    item.name = ui->nameEdit->text().trimmed();
    item.address = ui->addressEdit->toPlainText().trimmed();
    item.url = ui->urlEdit->text().trimmed();

    if (item.name.isEmpty()) {
        QMessageBox::warning(this, "Peringatan", "Nama tempat harus diisi.");
        ui->nameEdit->setFocus();
        return;
    }

    db::saveLocation(item);

    QDialog::accept();
}

LocationManager::LocationManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocationManager),
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

LocationManager::~LocationManager()
{
    delete ui;
}

Location *LocationManager::selectedItem() const
{
    QModelIndex current = ui->tableView->currentIndex();
    if (!current.isValid())
        return nullptr;

    return &model->items[proxyModel->mapToSource(current).row()];
}

void LocationManager::add()
{
    Editor editor(this);
    editor.setWindowTitle("Tambah Lokasi");
    if (!editor.exec())
        return;

    refresh();

    emit dataChanged();
}

void LocationManager::edit()
{
    Model::Item *item = selectedItem();
    if (!item)
        return;

    QString name = item->name;
    name.truncate(20);

    Editor editor(this);
    editor.setWindowTitle(QString("Edit %1..").arg(name));
    editor.edit(item);
    if (!editor.exec())
        return;

    refresh();

    emit dataChanged();
}

void LocationManager::remove()
{
    Model::Item *item = selectedItem();
    if (!item)
        return;

    if (QMessageBox::question(this, "Konfirmasi", "Hapus rekaman yang dipilih?") != QMessageBox::Yes)
        return;

    db::removeLocation(*item);

    refresh();

    emit dataChanged();
}

void LocationManager::refresh()
{
    model->refresh();
    ui->infoLabel->setText(QString("Menampilkan %1 rekaman.")
                           .arg(locale().toString(model->rowCount())));
}

