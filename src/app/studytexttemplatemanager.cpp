#include "studytexttemplatemanager.h"
#include "ui_studytexttemplatemanager.h"
#include "ui_studytexttemplateeditor.h"
#include "db.h"

#include <QSortFilterProxyModel>
#include <QMessageBox>

class StudyTextTemplateManager::Model : public QAbstractTableModel
{
public:
    typedef StudyTextTemplate Item;
    QList<Item> items;

    Model(QObject *parent) : QAbstractTableModel(parent) {}

    void refresh()
    {
        beginResetModel();
        items = db::getAllStudyTextTemplatesOrderById();
        endResetModel();
    }

    QVariant headerData(int section, Qt::Orientation o, int role) const
    {
        if (o == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
            case 0: return "Nama Template";
            case 1: return "Pratinjau";
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
                case 1: return item.content;
                }
            }
        }
        return QVariant();
    }
};

StudyTextTemplateManager::StudyTextTemplateManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StudyTextTemplateManager),
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

StudyTextTemplateManager::Editor::Editor(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::StudyTextTemplateEditor)
    , id(0)
{
    ui->setupUi(this);

    connect(ui->saveButton, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
}

StudyTextTemplateManager::Editor::~Editor()
{
    delete ui;
}

void StudyTextTemplateManager::Editor::edit(StudyTextTemplate *item)
{
    id = item->id;
    ui->nameEdit->setText(item->name);
    ui->templateEdit->setPlainText(item->content);
}

void StudyTextTemplateManager::Editor::accept()
{
    StudyTextTemplate item;

    item.id = id;
    item.name = ui->nameEdit->text().trimmed();
    item.content = ui->templateEdit->toPlainText().trimmed();

    if (item.name.isEmpty()) {
        QMessageBox::warning(this, "Peringatan", "Nama ustadz harus diisi.");
        ui->nameEdit->setFocus();
        return;
    }

    db::saveStudyTextTemplate(item);

    QDialog::accept();
}

StudyTextTemplateManager::~StudyTextTemplateManager()
{
    delete ui;
}

StudyTextTemplate *StudyTextTemplateManager::selectedItem() const
{
    QModelIndex current = ui->tableView->currentIndex();
    if (!current.isValid())
        return nullptr;

    return &model->items[proxyModel->mapToSource(current).row()];
}

void StudyTextTemplateManager::add()
{
    Editor editor(this);
    editor.setWindowTitle("Tambah Ustadz");
    if (!editor.exec())
        return;

    refresh();

    emit dataChanged();
}

void StudyTextTemplateManager::edit()
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

void StudyTextTemplateManager::remove()
{
    Model::Item *item = selectedItem();
    if (!item)
        return;

    if (QMessageBox::question(this, "Konfirmasi", "Hapus rekaman yang dipilih?") != QMessageBox::Yes)
        return;

    db::removeStudyTextTemplate(*item);

    refresh();

    emit dataChanged();
}

void StudyTextTemplateManager::refresh()
{
    model->refresh();
    ui->infoLabel->setText(QString("Menampilkan %1 rekaman.")
                           .arg(locale().toString(model->rowCount())));
}
