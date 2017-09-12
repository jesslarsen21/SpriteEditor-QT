/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog *ui;

signals:
    void tellViewToClose(std::string input);
};

#endif // DIALOG_H
