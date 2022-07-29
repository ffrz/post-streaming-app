#ifndef PREVIEWTEXTDIALOG_H
#define PREVIEWTEXTDIALOG_H

#include <QDialog>

namespace Ui {
class PreviewTextDialog;
}

class PreviewTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewTextDialog(QWidget *parent = nullptr);
    ~PreviewTextDialog();

    void setTemplateName(const QString &name);
    void setTextContent(const QString &content);

private:
    Ui::PreviewTextDialog *ui;
};

#endif // PREVIEWTEXTDIALOG_H
