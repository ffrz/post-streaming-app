#include "previewtextdialog.h"
#include "ui_previewtextdialog.h"

PreviewTextDialog::PreviewTextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewTextDialog)
{
    ui->setupUi(this);

    connect(ui->closeButton, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->selectAllButton, SIGNAL(clicked()), ui->contentEdit, SLOT(selectAll()));
    connect(ui->copyButton, SIGNAL(clicked()), ui->contentEdit, SLOT(copy()));
}

PreviewTextDialog::~PreviewTextDialog()
{
    delete ui;
}

void PreviewTextDialog::setTemplateName(const QString &name)
{
    ui->templateNameLabel->setText(QString("Nama Template: %1").arg(name));
}

void PreviewTextDialog::setTextContent(const QString &content)
{
    ui->contentEdit->setPlainText(content);
}
