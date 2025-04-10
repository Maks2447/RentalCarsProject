#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "database.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <registrdialog.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadCars(QString queryStr);

    void setData(const UserData &user);

private slots:

    void resetFilters_clicked();

    void on_LoginButton_clicked();

    void on_setFilters_pushButton_clicked();

    QString getFilters();

    void clearCars();

    void applyStyleSheet();

private:
    Ui::MainWindow *ui;

    Database db;
    UserData currentUser;

    QWidget *container;
    QVBoxLayout *layout;

    UserData user;
    QMenu *menu;
};
#endif // MAINWINDOW_H
