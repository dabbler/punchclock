#ifndef VIEWENTRIES_H
#define VIEWENTRIES_H

#include <QDialog>

namespace Ui {
class ViewEntries;
}

class ViewEntries : public QDialog
{
    Q_OBJECT

public:
    explicit ViewEntries(QWidget *parent = 0);
    ~ViewEntries();

private:
    Ui::ViewEntries *ui;

private slots:
    void on_butOk_clicked();
    void on_butCancel_clicked();
};

#endif // VIEWENTRIES_H
