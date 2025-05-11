#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "database.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <registrdialog.h>
#include <QCalendarWidget>
#include <QFormLayout>
#include <QLabel>

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

    QPixmap roundedPixmap(const QPixmap photoPixmap, int width, int height);

    void on_Home_DayFrom_pushButton_clicked();

    void calendarDataChoice(const QDate &date);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void on_Home_DayTo_pushButton_clicked();

    void addRowsTime(QMenu &menu, QToolButton *name);

    void on_Home_search_pushButton_clicked();

    void on_Order_button_clicked();

    void set_validator();

    void show_active_orders();

    void show_past_orders();

    void create_widgetCard(QVBoxLayout *layoutWidgetCard, QVector<QPair<QVector<QString>, QPixmap>> &carsList, bool isActive);

private:
    Ui::MainWindow *ui;

    Database db;
    UserData currentUser;

    QWidget *container;
    QVBoxLayout *layout;

    //UserData user;
    QMenu *menu;

    QCalendarWidget *calendar;

    bool isLogin;

    QDate calendar_start_date;
    QDate calendar_end_date;
    QDate start_date;
    QDate end_date;

    QString timePick_start;
    QString timePick_end;

    QString car_id;

    QString oldStyle;
};
#endif // MAINWINDOW_H
