#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "global.h"

#include <QApplication>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowCloseButtonHint),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    setWindowTitle(QString("Tentang %1").arg(APP_DISPLAY_NAME));

    ui->infoLabel->setTextFormat(Qt::RichText);
    ui->infoLabel->setText(QString(
                                  "<h3>%1 %2</h3>"
                                  "<p>Berbasis Qt %3</p>"
                                  "<p>© 2021 Shift IT Solution</p>")
                              .arg(APP_DISPLAY_NAME, APP_VERSION_STR, QT_VERSION_STR));

    connect(ui->closeButton, SIGNAL(clicked()), SLOT(accept()));
    adjustSize();
    setFixedSize(size());
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
