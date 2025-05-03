#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "database.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <registrdialog.h>
#include <QCalendarWidget>

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

    void orderCarShow(const QVector<QString>& carData, const QPixmap &photoPixmap);

    QPixmap roundedPixmap(const QPixmap &src, int radius);

    void onSelectionChanged();

    void on_Home_DayFrom_pushButton_clicked();

    void calendarDataChoice(const QDate &date);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void on_Home_DayTo_pushButton_clicked();

    void addRowsTime(QMenu &menu, QString name);

private:
    Ui::MainWindow *ui;

    Database db;
    UserData currentUser;

    QWidget *container;
    QVBoxLayout *layout;

    UserData user;
    QMenu *menu;

    QCalendarWidget *calendar;

    bool isLogin;
};
#endif // MAINWINDOW_H
