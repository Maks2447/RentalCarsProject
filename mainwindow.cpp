#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"
#include "registrdialog.h"
#include "UserData.h"

#include <QFileDialog>
#include <QLabel>
#include <QStackedWidget>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QCalendarWidget>
#include <QWidgetAction>
#include <QRegularExpressionValidator>
#include <QTabBar>
#include <QFormLayout>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isLogin(false)
{    
    ui->setupUi(this);

    if (db.openDatabase()) {
        //qDebug() << "База данных открыта успешно!";
    }

    container = new QWidget();
    layout = new QVBoxLayout(container);
    container->setLayout(layout);

    ui->scrollArea->setWidget(container);
    ui->scrollArea->setWidgetResizable(true);

    layout->setSpacing(8);
    layout->setContentsMargins(0, 15, 0, 0);

    QHBoxLayout *loginLayout = new QHBoxLayout(ui->LoginButton);
    loginLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *loginIcon = new QLabel(ui->LoginButton);

    loginLayout->addWidget(loginIcon, 0, Qt::AlignLeft);

    ui->LoginButton->setLayout(loginLayout);

    QPixmap pixmap("C:/Users/golov/Downloads/icons8-человек-30.png");
    pixmap = pixmap.scaled(QSize(20,20),Qt::KeepAspectRatio);
    loginIcon->setPixmap(pixmap);
    loginIcon->setMaximumWidth(20);


    //
    calendar = new QCalendarWidget(this);
    calendar->setWindowFlags(Qt::Popup);
    calendar->installEventFilter(this);
    //

    this->setObjectName("this");
    ui->LoginButton->setObjectName("LoginButton");
    container->setObjectName("container");
    ui->setFilters_pushButton->setObjectName("setFilters");
    ui->OrderPage->setObjectName("OrderPage");
    oldStyle = ui->Order_age_checkBox->styleSheet();
    calendar->setObjectName("calendar");

    connect(ui->backToHomePage, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    //connect(ui->Home_searchTo_pushButton, &QPushButton::clicked, this, &MainWindow::on_Home_searchFrom_pushButton_clicked);
    connect(calendar, &QCalendarWidget::clicked, this, &MainWindow::calendarDataChoice);
    ui->Home_DayFrom_pushButton->setText(QDate::currentDate().toString("MMM dd"));
    calendar_start_date = QDate::currentDate();
    ui->Home_DayTo_pushButton->setText(QDate::currentDate().addDays(1).toString("MMM dd"));
    calendar_end_date = QDate::currentDate().addDays(1);

    QMenu *timePick_startMenu = new QMenu;
    addRowsTime(*timePick_startMenu, ui->Home_timePick_start);
    QMenu *timePick_endMenu = new QMenu;
    addRowsTime(*timePick_endMenu, ui->Home_timePick_end);

    ui->Home_timePick_start->setMenu(timePick_startMenu);
    ui->Home_timePick_end->setMenu(timePick_endMenu);

    loadCars("");
    setData(currentUser);
    applyStyleSheet();
    set_validator();

    //calendar->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

    // Стилизация календаря (чтобы он был красивее)
    // calendar->setStyleSheet(
    //     "QCalendarWidget {"
    //     "    background-color: #212121;"
    //     "    color: white;"
    //     "    border: 1px solid #444;"
    //     "    border-radius: 8px;"
    //     "}"
    //     "QCalendarWidget QAbstractItemView:enabled {"
    //     "    background-color: #212121;"
    //     "    selection-background-color: #2E7D32;"
    //     "    selection-color: white;"
    //     "    gridline-color: #555;"
    //     "}"
    //     "QCalendarWidget QToolButton {"
    //     "    background-color: #2E7D32;"
    //     "    color: white;"
    //     "    font-weight: bold;"
    //     "    border-radius: 4px;"
    //     "    margin: 5px;"
    //     "}"
    //     );


    ui->Order_iconLittleCar1->setPixmap(QPixmap("C:/Users/golov/Downloads/icons8-автомобиль-40"));
    ui->Order_iconLittleCar1->setScaledContents(true);
    ui->Order_icon_line->setPixmap(QPixmap("C:/Users/golov/Downloads/icons8-вертикальная-линия-50"));
    ui->Order_icon_line->setScaledContents(true);
    ui->Order_iconLittleCar2->setPixmap(QPixmap("C:/Users/golov/Downloads/icons8-автомобиль-40"));
    ui->Order_iconLittleCar2->setScaledContents(true);

    ui->Home_DayFrom_pushButton->setIcon(QPixmap("C:/Users/golov/Downloads/icons8-календарь-24.png"));
    ui->Home_DayTo_pushButton->setIcon(QPixmap("C:/Users/golov/Downloads/icons8-календарь-24.png"));

    ui->Home_search_pushButton->setIcon(QPixmap("C:/Users/golov/Downloads/icons8-поиск-50.png"));

    ui->Home_arrow_label->setPixmap(QPixmap("C:/Users/golov/Downloads/icons8-стрелка-50 (1).png"));
    ui->Home_arrow_label->setScaledContents(true);

    ui->backToHomePage->setIcon(QIcon("C:/Users/golov/Downloads/icons8-back-52"));

    for(QPushButton *button : this->findChildren<QPushButton*>()) {
        button->setCursor(Qt::PointingHandCursor);
    }
    for(QCheckBox *checkBox : this->findChildren<QCheckBox*>()) {
        checkBox->setCursor(Qt::PointingHandCursor);
    }

    connect(ui->Account_innerTab, &QTabWidget::currentChanged, this, &MainWindow::changeCurrentTab);

    on_Home_search_pushButton_clicked();

    int index = ui->ProfilePage_mainTabWidget->indexOf(ui->tab);
    if (index != -1) {
        ui->ProfilePage_mainTabWidget->removeTab(index);
    }
    ui->ProfilePage_mainTabWidget->insertTab(index, ui->tab, "My Tab");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addRowsTime(QMenu &menu, QToolButton *name) {

    QWidget *widgetMenu = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widgetMenu);
    layout->setContentsMargins(0, 0, 0, 0);
    widgetMenu->setLayout(layout);
    layout->setSpacing(0);

    QWidget *titleWidget = new QWidget();
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleWidget->setLayout(titleLayout);
    titleWidget->setStyleSheet("border-bottom: 1px solid white;");
    titleLayout->setContentsMargins(40, 10, 10, 10);

    QWidget *widgetButtons = new QWidget();
    QGridLayout *layoutButtons = new QGridLayout(widgetButtons);
    widgetButtons->setLayout(layoutButtons);
    layoutButtons->setContentsMargins(10, 10, 10, 10);

    layout->addWidget(titleWidget);
    layout->addWidget(widgetButtons);


    QLabel *titleTextLabel = new QLabel("Select pickup time");
    titleTextLabel->setStyleSheet("font-size: 18px;");
    titleTextLabel->setAlignment(Qt::AlignLeft);
    QLabel *titleIconLabel = new QLabel();
    titleIconLabel->setPixmap(QPixmap("C:/Users/golov/Downloads/icons8-часы-32.png"));
    titleIconLabel->setAlignment(Qt::AlignRight);
    titleIconLabel->setFixedSize(20,20);
    titleIconLabel->setScaledContents(true);

    titleLayout->addWidget(titleIconLabel);
    titleLayout->addWidget(titleTextLabel);


    QStringList timeListMorning = {
        "7:00 AM", "8:00 AM", "9:00 AM", "10:00 AM", "11:00 AM", "12:00 PM"
    };
    QStringList timeListAfternoon = {
        "1:00 PM", "2:00 PM", "3:00 PM", "4:00 PM", "5:00 PM", "6:00 PM", "7:00 PM", "8:00 PM"
    };

    int row = 0, col = 0;


    QLabel *morningLabel = new QLabel("Morning");
    layoutButtons->addWidget(morningLabel, row, 0, 1, 2);
    row++;

    for(int i = 0; i < timeListMorning.size(); i++) {
        QPushButton *btn = new QPushButton(timeListMorning[i]);
        btn->setFixedSize(115,42);
        layoutButtons->addWidget(btn, row, col);

        connect(btn, &QPushButton::clicked, this, [btn, name, &menu]() {
            menu.hide();
            name->setText(btn->text());
        });

        col++;
        if(col == 2) {
            col = 0;
            row++;
        }
    }

    row++;
    QLabel *afternoonLabel = new QLabel("Afternoon");
    layoutButtons->addWidget(afternoonLabel, row, 0, 1, 2);
    row++;

    col = 0;
    for(int i = 0; i < timeListAfternoon.size(); i++) {
        QPushButton *btn = new QPushButton(timeListAfternoon[i]);
        btn->setFixedSize(115,42);
        layoutButtons->addWidget(btn, row, col);

        connect(btn, &QPushButton::clicked, this, [btn, name, &menu]() {
            menu.hide();
            name->setText(btn->text());
        });

        col++;
        if(col == 2) {
            col = 0;
            row++;
        }
    }

    QWidgetAction *widgetAction = new QWidgetAction(&menu);
    widgetAction->setDefaultWidget(widgetMenu);
    menu.addAction(widgetAction);

    menu.setStyleSheet("QMenu {"
                       "    background-color: #212121;"
                       "}"
                       "QMenu *{"
                       "    color: white;"
                       "    font-size: 16px;"
                       "    font-weight: 600;"
                       "}"
                       "QPushButton {"
                       "    background-color: orange;"
                       "    border-radius: 8px;"
                       "}"
                       "QPushButton::pressed {"
                       "    background-color: #d67600;"
                       "}");

    connect(&menu, &QMenu::aboutToShow, this, [=]() {
        name->setStyleSheet("QToolButton {"
                            "   border: 1px solid white;"
                            "   border-radius: 8px;"
                            "   border-top-left-radius: 0px;"
                            "   border-bottom-left-radius: 0px;"
                            "   padding: 4px;"
                            "}"
                            "QToolButton:pressed {"
                            "    border: 2px solid orange;"
                            "}"
                            "QToolButton:checked {"
                            "    border: 2px solid orange;"
                            "}");
    });

    connect(&menu, &QMenu::aboutToHide, this, [=]() {
        name->setStyleSheet("QToolButton {"
                            "border: 1px solid white;"
                            "border-radius: 8px;"
                            "border-top-left-radius: 0px;"
                            "border-bottom-left-radius: 0px;"
                            "padding: 4px;"
                            "}");
    });

}

void MainWindow::loadCars(QString queryStr)
{
    clearCars();
    QString iconGas = "C:/Users/golov/Downloads/gasstation_icon.png";
    QString iconTransm = "C:/Users/golov/Downloads/icons8-селектор-коробки-передач-64.png";
    QString iconPassengers = "C:/Users/golov/Downloads/icons8-человек-64.png";
    QString iconClas = "C:/Users/golov/Downloads/icons8-руль-24.png";;

    QVector<QPair<QVector<QString>, QPixmap>> cars;

    if(queryStr.isEmpty()) {
        cars = db.getCars("");
    } else {
        cars = db.getCars(queryStr);
    }

    if(cars.isEmpty()) {
        QWidget *emptySearchWdg = new QWidget(container);
        QGridLayout *emptySearchLayout = new QGridLayout(emptySearchWdg);

        QLabel *iconEmptySearch = new QLabel(emptySearchWdg);
        QPixmap iconPixmap("C:/Users/golov/Downloads/free-icon-sad-face-4989793.png");
        iconEmptySearch->setPixmap(iconPixmap);
        iconEmptySearch->setScaledContents(true);
        iconEmptySearch->setMaximumWidth(37);
        iconEmptySearch->setMaximumHeight(37);

        QLabel *emptyLabelTitle = new QLabel("No cars available", emptySearchWdg);
        QLabel *emptyLabel = new QLabel("No vehicles match your search criteria. Try again with less filters", emptySearchWdg);
        QPushButton *resetFilters = new QPushButton("Reset filters", emptySearchWdg);

        connect(resetFilters, &QPushButton::clicked, this, &MainWindow::resetFilters_clicked);

        emptySearchLayout->addWidget(iconEmptySearch,0,0,2,1);
        emptySearchLayout->addWidget(emptyLabelTitle,0,1);
        emptySearchLayout->addWidget(emptyLabel,1,1);
        emptySearchLayout->addWidget(resetFilters,0,2,2,1, Qt::AlignRight);

        emptySearchLayout->setVerticalSpacing(0);
        emptySearchLayout->setHorizontalSpacing(10);
        emptyLabelTitle->setMaximumHeight(25);

        resetFilters->setMaximumHeight(20);
        resetFilters->setMaximumWidth(95);

        emptySearchWdg->setLayout(emptySearchLayout);
        layout->addWidget(emptySearchWdg);

        emptySearchWdg->setFixedHeight(70);
        emptySearchWdg->setFixedWidth(1000);

        emptySearchWdg->setObjectName("emptySearchWdg");
        emptyLabelTitle->setObjectName("emptyLabelTitle");
        emptyLabel->setObjectName("emptyLabel");
        resetFilters->setObjectName("resetFilters");
    }

    for(int i = 0; i < cars.size(); ++i) {
        const QVector<QString> &car = cars[i].first;
        const QPixmap &photoPixmap = cars[i].second;

        QWidget *childWdg = new QWidget(container);

        QGridLayout * childLayout = new QGridLayout(childWdg);
        QSpacerItem *spacerTop = new QSpacerItem(0, 10);
        QSpacerItem *spacerLower = new QSpacerItem(0, 290);

        //Add objects in layout
        QLabel *photo = new QLabel("Photo", childWdg);

        QLabel *model = new QLabel(car[0], childWdg);
        QLabel *descr = new QLabel(car[1], childWdg);

        QLabel *status = new QLabel(childWdg);
        if(car[2] == "true") {
            status->setText("Available");
            status->setFixedWidth(75);
        } else {
            status->setText("Unavailable");
            status->setFixedWidth(96);
        }

        QLabel *fuel = new QLabel("<img src='" + iconGas + "' style='vertical-align: bottom;' width='20' height='20'> " + car[3], childWdg);
        QLabel *gearbox = new QLabel("<img src='" + iconTransm + "' style='vertical-align: bottom;' width='20' height='20'> " + car[4], childWdg);

        QLabel *carClas = new QLabel("<img src='"+ iconClas + "' style='vertical-align: bottom;' width='18' height='18'> " + car[6], childWdg);
        QLabel *passengers = new QLabel("<img src='" + iconPassengers + "' style='vertical-align: top;' width='20' height='20'> " + car[7], childWdg);

        QLabel *price = new QLabel("<span style='font-size: 20px;'>" + car[5] + "</span>"
                                   "<span style='font-size: 17px;'>usd/day</span>", childWdg);
        price->setContentsMargins(0, 5, 0, 0);

        QPushButton *orderButton = new QPushButton("Order", childWdg);

        connect(orderButton, &QPushButton::clicked, this, [=]() {
            orderCarShow(car, photoPixmap);
        });

        QLabel *space = new QLabel("", childWdg);
        //>>

        //Adding widgets in grid(positions)
        childLayout->addWidget(photo, 0,0, 7,1, Qt::AlignLeft);
        childLayout->addWidget(model, 0,1, 1,2);
        childLayout->addWidget(descr, 1,1, 1,2, Qt::AlignTop);
        childLayout->addItem(spacerTop, 2, 1);
        childLayout->addWidget(status, 3,1);
        childLayout->addWidget(gearbox, 4,1, Qt::AlignBottom);
        childLayout->addWidget(fuel, 5,1);
        childLayout->addItem(spacerLower, 6, 1);

        childLayout->addWidget(carClas, 4, 2, Qt::AlignBottom);
        childLayout->addWidget(passengers, 5,2, Qt::AlignTop);

        childLayout->addWidget(price, 0,3, Qt::AlignRight);
        childLayout->addWidget(orderButton, 6,3, Qt::AlignRight);
        childLayout->addWidget(space, 5,3);
        //>>

        //Set picture
        photo->setPixmap(photoPixmap);
        photo->setScaledContents(true);
        photo->setFixedWidth(330);
        childLayout->setColumnMinimumWidth(0, 338);
        childLayout->setColumnMinimumWidth(1, 106);
        //>>

        space->setFixedWidth(480);
        //Relative(относительный) distance between rows
        childLayout->setRowStretch(0, 0);
        childLayout->setRowStretch(1, 0);
        childLayout->setRowStretch(2, 0);
        childLayout->setRowStretch(3, 0);
        childLayout->setRowStretch(4, 2);
        childLayout->setRowStretch(5, 2);
        childLayout->setRowStretch(6, 4);
        //Columns
        childLayout->setColumnStretch(0,0);
        childLayout->setColumnStretch(1,3);
        childLayout->setColumnStretch(2,1);
        childLayout->setColumnStretch(3,1);
        //>>

        childLayout->setVerticalSpacing(1);
        childLayout->setContentsMargins(10, 10, 23, 10);

        childWdg->setFixedHeight(210);
        childWdg->setFixedWidth(1060);
        childWdg->setLayout(childLayout);
        childWdg->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        layout->addWidget(childWdg);

        orderButton->setFixedHeight(35);
        orderButton->setFixedWidth(200);
        gearbox->setMinimumHeight(25);
        carClas->setMinimumHeight(25);
        passengers->setMaximumHeight(19);
        fuel->setMaximumHeight(25);

        //Names for css file:
        childWdg->setObjectName("childWdg");
        childLayout->setObjectName("childLayout");
        model->setObjectName("model");
        descr->setObjectName("descr");
        status->setObjectName("status");
        gearbox->setObjectName("gearbox");
        fuel->setObjectName("fuel");
        price->setObjectName("price");
        orderButton->setObjectName("orderButton");
        passengers->setObjectName("passengers");
        carClas->setObjectName("carClas");
        status->setObjectName("status");
        orderButton->setObjectName("orderButton");
        //>> 
    }
    layout->addStretch();
}

void MainWindow::setData(const UserData &user)
{
    currentUser = user;
    isLogin = true;

    currentUser.phone = "+48 790504452";
    currentUser.name = "Maksym";
    currentUser.surname = "Holoviznyi";
    currentUser.email = "golovisnyimaksim@gmail.com";

    double loginButtonWidth;

    QString UserName = currentUser.name + " " +currentUser.surname;
    UserName = "Maksym Holoviznyi";
    QFontMetrics fm(ui->LoginButton->font());
    int textWidth = fm.horizontalAdvance(UserName);

    loginButtonWidth = textWidth + 30;

    ui->LoginButton->setMinimumWidth(loginButtonWidth);
    ui->LoginButton->setMaximumWidth(loginButtonWidth);

    ui->LoginButton->setText(UserName);

    ui->logoName_label->installEventFilter(this);

    QMenu *menu = new QMenu(this);

    QAction *bookings = menu->addAction("Bookings");
    bookings->setIcon(QIcon("C:/Users/golov/Downloads/exclamationmark_white.png"));

    QAction *account = menu->addAction("Account");
    account->setIcon(QIcon("C:/Users/golov/Downloads/icons8-руль-24.png"));

    menu->addSeparator();
    QAction *logout = menu->addAction("logout");

    connect(bookings, &QAction::triggered, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->ProfilePage);
        ui->ProfilePage_mainTabWidget->setCurrentWidget(ui->Profile_Booking_Tab);
    });

    QTabBar *tabBar = ui->Booking_innerTab->tabBar();

    int index = ui->Booking_innerTab->insertTab(0,new QWidget(), "My bookings");
    ui->Booking_innerTab->setTabEnabled(index, false);
    ui->Booking_innerTab->setCurrentIndex(1);
    tabBar->setStyleSheet("QTabBar::tab:first {background: transparent; color: white; font: 32px; margin-top: -5px; padding-top: -0px; padding-right: 15px; padding-left: -35px;}");

    connect(tabBar, &QTabBar::currentChanged, this, [=](int index){
        if (index == 1) {
            ui->Booking_innerTab->setCurrentIndex(1);
        } else {
            ui->Booking_innerTab->setCurrentIndex(2);
        }
    });

    connect(account, &QAction::triggered, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->ProfilePage);
        ui->ProfilePage_mainTabWidget->setCurrentWidget(ui->Profile_account_Tab);
    });

    connect(logout, &QAction::triggered, this, [=](){
        QMessageBox::information(this, "Заказы", "Показать заказы");
    });
    menu->setMinimumWidth(loginButtonWidth);

    connect(ui->LoginButton, &QPushButton::clicked, this, [=]() {
        QPoint pos = ui->LoginButton->mapToGlobal(QPoint(ui->LoginButton->width(), 0));

        int menuWidth = menu->sizeHint().width();
        pos.setX(pos.x() - menuWidth);
        pos.setY(pos.y() + 30);

        menu->popup(pos);
    });

    menu->setStyleSheet(
        "QMenu {"
        "   background-color: #212121;"
        "   padding: 10px 0px 10px 0px; "
        "   width: 220px;"
        "}"

        "QMenu::icon {"
        "   padding-left: 20px;"
        "}"

        "QMenu::item {"
        "   height: 30px;"
        "   padding: 7px 10px;"
        "   padding-left: 15px;"
        "   font-size: 18px;"
        "}"

        "QMenu::item:selected {"
        "   background-color: #1c1c1c;"
        "}"

        "QMenu::separator {"
        "   height: 2px;"
        "   background: #444;"
        "   margin: 5px 0;"
        "}"
        );


    show_active_orders();
    show_past_orders();

    creationInnerTabs();


}


void MainWindow::applyStyleSheet()
{
    QFile file(":/mainStyle.css");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
}

void MainWindow::orderCarShow(const QVector<QString>& carData, const QPixmap &photoPixmap)
{
    int rental_days = start_date.daysTo(end_date);
    ui->Order_rentalDays->setText(QString::number(rental_days) + " rental days");
    ui->Order_numberOfDays_label->setText(QString::number(rental_days) + " days");

    double price = carData[5].toInt() * rental_days;
    car_id = carData[8];

    ui->Order_pickup_data->setText(start_date.toString("ddd, MMM d, yyyy") + " | " + timePick_start);
    ui->Order_return_data->setText(end_date.toString("ddd, MMM d, yyyy") + " | " + timePick_end);

    ui->stackedWidget->setCurrentWidget(ui->OrderPage);
    qDebug() << currentUser.name;
    if(isLogin) {
        ui->Order_name_lineEdit->setText(currentUser.name);
        ui->Order_lastName_lineEdit->setText(currentUser.surname);
        ui->Order_email_lineEdit->setText(currentUser.email);
        ui->Order_phone_lineEdit->setText(currentUser.phone);
    }

    QPixmap rounded = (roundedPixmap(photoPixmap, 300, 300));
    ui->Order_photo->setPixmap(rounded);
    ui->Order_photo->setMaximumWidth(110);
    ui->Order_photo->setMaximumHeight(70);
    ui->Order_photo->setScaledContents(true);


    ui->Order_carModel->setText(carData[0]);
    ui->Order_price_label->setText("$" + QString::number(price));
    ui->Order_top_totalPrice_label->setText("<span style='font-size: 20px;'> Total:</span>"
                                            "<span style='font-size: 28px;'>"" $" + QString::number(price) + "</span>");
}

void MainWindow::on_Home_search_pushButton_clicked()
{
    start_date = calendar_start_date;
    end_date = calendar_end_date;
    timePick_start = ui->Home_timePick_start->text();
    timePick_end = ui->Home_timePick_end->text();
    qDebug() << start_date << start_date <<  timePick_start << timePick_end;

    QString queryStr = QString(
                           "SELECT * FROM \"Cars\" WHERE \"id_car\" NOT IN ("
                           "SELECT \"car_id\" FROM \"Orders\" "
                           "WHERE NOT ('%1' > \"end_date\" OR '%2' < \"start_date\"))"
                           ).arg(start_date.toString("yyyy-MM-dd"), end_date.toString("yyyy-MM-dd"));

    loadCars(queryStr);
}

QPixmap MainWindow::roundedPixmap(const QPixmap photoPixmap, int width, int height)
{

    QPixmap image = photoPixmap.scaled(1800, 1200, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap clipImage(image.size());
    clipImage.fill(Qt::transparent);

    QPainter painter(&clipImage);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath clip;
    clip.addRoundedRect(image.rect(), width, height);

    painter.setClipPath(clip);
    painter.drawPixmap(0, 0, image);
    painter.end();

    return clipImage;
}

void MainWindow::on_LoginButton_clicked()
{
    if(isLogin) {
        return;
    }
    RegistrDialog RegistrForm(this);

    RegistrForm.setFixedSize(500, 500);
    RegistrForm.exec();
}

QString MainWindow::getFilters()
{
    QStringList selectModel;
    const auto &checkboxModel = ui->CarBrand_groupBox->findChildren<QCheckBox*>();
    for(QCheckBox *checkbox : checkboxModel) {
        if (checkbox->isChecked()) {
            selectModel.append(checkbox->text());
        }
    }

    QStringList selectPrice;
    const auto &checkboxPrice = ui->price_groupBox->findChildren<QCheckBox*>();
    for(QCheckBox *checkbox : checkboxPrice) {
        if (checkbox->isChecked()) {
            selectPrice.append(checkbox->text());
        }
    }

    QStringList selectPassengers;
    const auto &checkboxPassengers = ui->passengers_groupBox->findChildren<QCheckBox*>();
    for(QCheckBox *checkbox : checkboxPassengers) {
        if (checkbox->isChecked()) {
            selectPassengers.append(checkbox->text());
        }
    }

    QStringList selectVehicleType;
    const auto &checkboxVehicle = ui->vehicle_groupBox->findChildren<QCheckBox*>();
    for(QCheckBox *checkbox : checkboxVehicle) {
        if (checkbox->isChecked()) {
            selectVehicleType.append(checkbox->text());
        }
    }

    QString queryStr = "SELECT * FROM \"Cars\" WHERE (";
    QString queryDupl = queryStr + ")";
    QStringList generalCondition;
    QString str;

    if(!selectModel.isEmpty()) {
        QStringList modelCondition;
        for (const QString &model : selectModel) {
            modelCondition.append(QString("model LIKE '%%1%'").arg(model));
        }

        str += modelCondition.join(" OR ");
        generalCondition += str;

        qDebug() << generalCondition;
    }

    QStringList transCondition;
    if(ui->Manual_checkBox->isChecked()) {
        transCondition += "'Manual'";
    }
    if(ui->Automat_checkBox->isChecked()) {
        transCondition += "'Automat'";
    }
    if(ui->Electric_checkBox->isChecked()) {
        transCondition += "'Electric'";
    }

    if(!transCondition.isEmpty()) {
        generalCondition += "transmission IN (" + transCondition.join(",") + ")";
    }


    QMap<QString, QString> priceMappings = {
        {"$0 - $50", "price BETWEEN 0 AND 50"},
        {"$50 - $150", "price BETWEEN 50 AND 150"},
        {"$150 - $300", "price BETWEEN 150 AND 300"},
        {"$300 +", "price >= 300"}
    };

    if(!selectPrice.isEmpty()) {
        QStringList priceCondition;
        for (const QString &price : selectPrice) {
            priceCondition.append(priceMappings[price]);
        }
        generalCondition += priceCondition.join(" OR ");
    }

    if(!selectPassengers.isEmpty()) {
        QStringList passengersCondition;
        for (const QString &passengers : selectPassengers) {
            passengersCondition.append(QString("passengers = %1").arg(passengers));
        }

        str += passengersCondition.join(" OR ");
        generalCondition += str;

        qDebug() << generalCondition;
    }

    if(!selectVehicleType.isEmpty()) {
        QStringList VehicleTypeCondition;
        for (const QString &passengers : selectVehicleType) {
            VehicleTypeCondition.append(QString("type = '%1'").arg(passengers));
        }

        str += VehicleTypeCondition.join(" OR ");
        generalCondition += str;

        qDebug() << generalCondition;
    }


    queryStr += generalCondition.join(") AND (") + ")";
    qDebug() << queryStr;

    if(queryStr == queryDupl) {
        queryStr = "";
    }

    return queryStr;
}

void MainWindow::clearCars()
{
    while(QLayoutItem *item = layout->takeAt(0)) {
        if(QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void MainWindow::resetFilters_clicked()
{
    for(QCheckBox *checkbox : ui->widget->findChildren<QCheckBox*>()) {
        checkbox->setChecked(false);
    }
    clearCars();
    loadCars("");
}

void MainWindow::on_setFilters_pushButton_clicked()
{
    loadCars(getFilters());
}

void MainWindow::on_Home_DayFrom_pushButton_clicked()
{
    applyStyleSheet();

    ui->Home_DayFrom_pushButton->setChecked(true);
    QDate today = QDate::currentDate();
    calendar->setMinimumDate(today);

    QPoint pos = ui->Home_DayFrom_pushButton->mapToGlobal(QPoint(0, ui->Home_DayFrom_pushButton->height()));
    calendar->move(pos);
    calendar->show();
    calendar->setMinimumWidth(500);
    calendar->setMinimumHeight(140);
}

void MainWindow::calendarDataChoice(const QDate &date)
{
    if(ui->Home_DayFrom_pushButton->isChecked()) {
        ui->Home_DayFrom_pushButton->setText(date.toString("MMM dd"));
        ui->Home_DayFrom_pushButton->setChecked(false);
        calendar_start_date = date;
        on_Home_DayTo_pushButton_clicked();

    } else {
        ui->Home_DayTo_pushButton->setText(date.toString("MMM dd"));
        calendar->hide();
        calendar_end_date = date;
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == calendar && event->type() == QEvent::Hide) {
        ui->Home_DayFrom_pushButton->setChecked(false);
        ui->Home_DayTo_pushButton->setChecked(false);
        return true;
    }

    if (watched == ui->logoName_label && event->type() == QEvent::MouseButtonPress) {
        ui->stackedWidget->setCurrentWidget(ui->HomePage);
        return true;
    }

    if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(watched)) {
        if (event->type() == QEvent::FocusIn) {
            lineEdit->setStyleSheet("color: orange;");
        } else if (event->type() == QEvent::FocusOut) {
            lineEdit->setStyleSheet("color: white;");  // Вернуть к обычному
        }
    }
}

void MainWindow::on_Home_DayTo_pushButton_clicked()
{
    applyStyleSheet();
    ui->Home_DayTo_pushButton->setChecked(true);

    QString text = ui->Home_DayFrom_pushButton->text();
    text += " " + QString::number(QDate::currentDate().year());
    QDate fromDate = QDate::fromString(text, "MMM dd yyyy");
    calendar->setMinimumDate(fromDate.addDays(1));

    QPoint pos = ui->Home_DayTo_pushButton->mapToGlobal(QPoint(0, ui->Home_DayTo_pushButton->height()));
    calendar->move(pos);
    calendar->show();
    calendar->setMinimumWidth(500);
    calendar->setMinimumHeight(140);
}

void MainWindow::on_Order_button_clicked()
{
    bool isEmpty = false;
    QSqlQuery query;

    for (QLineEdit* lineEdit : ui->stackedWidget->widget(1)->findChildren<QLineEdit*>()) {
        if(lineEdit->text().isEmpty() || lineEdit->text() == "/") {
            lineEdit->setStyleSheet("border: 1px solid red;");
            if(lineEdit->inputMask().isEmpty()) {
                lineEdit->setPlaceholderText("The field can't be blank");
            }
            isEmpty = true;
        } else {
            lineEdit->setStyleSheet("");
        }
    }

    if(!ui->Order_age_checkBox->isChecked()) {
        ui->Order_age_checkBox->setStyleSheet(oldStyle + "QCheckBox {color: red;}");
        return;
    } else {
        ui->Order_age_checkBox->setStyleSheet(oldStyle);
    }

    if(isEmpty) return;

    if(isLogin) {
        query.prepare("INSERT INTO \"Orders\" (\"user_id\", \"car_id\", \"start_date\", \"end_date\")"
                      "VALUES(:user_id, :car_id, :start_date, :end_date)");
        query.bindValue(":user_id", currentUser.id_user);
        query.bindValue(":car_id", car_id);
        query.bindValue(":start_date", start_date);
        query.bindValue(":end_date", end_date);
        query.exec();

    } else {
        QString name = ui->Order_name_lineEdit->text();
        QString lastName = ui->Order_lastName_lineEdit->text();
        QString email = ui->Order_email_lineEdit->text();
        QString phone = ui->Order_phone_lineEdit->text();

        query.prepare("INSERT INTO \"Orders\" (\"name\", \"surname\", \"email\", \"phone\", \"car_id\", \"start_date\", \"end_date\")"
                      "VALUES(:name, :surname, :email, :phone, :car_id, :start_date, :end_date)");
        query.bindValue(":name", name);
        query.bindValue(":surname", lastName);
        query.bindValue(":email", email);
        query.bindValue(":phone", phone);
        query.bindValue(":car_id", car_id);
        query.bindValue(":start_date", start_date);
        query.bindValue(":end_date", end_date);
        qDebug() << start_date;
        qDebug() << end_date;

        query.exec();
    }
    show_active_orders();
    ui->stackedWidget->setCurrentWidget(ui->HomePage);
}



void MainWindow::set_validator()
{
    //User order
    QRegularExpression nameExpr ("^[A-Za-zА-Яа-яЁё]{2,15}$");
    QRegularExpressionValidator *nameValidator = new QRegularExpressionValidator(nameExpr, this);
    ui->Order_name_lineEdit->setValidator(nameValidator);

    QRegularExpression surnameExpr ("^[A-Za-zА-Яа-яЁё]{2,15}$");
    QRegularExpressionValidator *surnameValidator = new QRegularExpressionValidator(surnameExpr, this);
    ui->Order_lastName_lineEdit->setValidator(surnameValidator);

    QRegularExpression phoneExpr ("^\\+?[0-9]{7,16}$");
    QRegularExpressionValidator *phoneValidator = new QRegularExpressionValidator(phoneExpr, this);
    ui->Order_phone_lineEdit->setValidator(phoneValidator);

    //User payment
    QRegularExpression cardNameExpr ("^\\d{16}$");
    QRegularExpressionValidator *cardNameValidator = new QRegularExpressionValidator(cardNameExpr, this);
    ui->Order_cardNumber_lineEdit->setValidator(cardNameValidator);

    QRegularExpression cardHolderExpr ("^[A-Za-zА-Яа-яЁё ]{5,30}$");
    QRegularExpressionValidator *cardHolderValidator = new QRegularExpressionValidator(cardHolderExpr, this);
    ui->Order_cardholderr_lineEdit->setValidator(cardHolderValidator);


    ui->Order_date_lineEdit->setInputMask("00/00;_");

    ui->Order_CVV_lineEdit->setInputMask("000;_");

}

void MainWindow::show_active_orders()
{
    QSqlQuery query;
    QDate currentDate = QDate::currentDate();
    query.prepare("SELECT \"Cars\".\"photo\", \"Cars\".\"model\", \"Cars\".\"price\", \"Orders\".\"start_date\", \"Orders\".\"end_date\"  "
                  "FROM \"Users\" "
                  "JOIN \"Orders\" ON \"Users\".\"id_user\" = \"Orders\".\"user_id\" "
                  "JOIN \"Cars\" ON \"Orders\".\"car_id\" = \"Cars\".\"id_car\" "
                  "WHERE \"Users\".\"id_user\" = :id_user "
                  "AND \"Orders\".\"start_date\" >= :currentDate");
    currentUser.id_user = 1;
    query.bindValue(":id_user", currentUser.id_user);
    query.bindValue(":currentDate", currentDate);

    QVector<QPair<QVector<QString>, QPixmap>> carsList;

    if(query.exec()) {
        while(query.next()) {
            QVector<QString> car;

            car.append(query.value("model").toString());
            car.append(query.value("price").toString());
            car.append(query.value("start_date").toDate().toString("dd MMM"));
            car.append(query.value("end_date").toDate().toString("dd MMM"));

            QPixmap pixmap;
            QByteArray photoData = query.value("photo").toByteArray();

            if(!photoData.isEmpty()) {
                pixmap.loadFromData(photoData);
            }

            carsList.append(qMakePair(car, pixmap));

            qDebug() << car;
        }
    }
    create_widgetCard(ui->verticalLayout_14, carsList, true);
}

void MainWindow::show_past_orders()
{
    QSqlQuery query;
    QDate currentDate = QDate::currentDate();
    query.prepare("SELECT \"Cars\".\"photo\", \"Cars\".\"model\", \"Cars\".\"price\", \"Orders\".\"start_date\", \"Orders\".\"end_date\"  "
                  "FROM \"Users\" "
                  "JOIN \"Orders\" ON \"Users\".\"id_user\" = \"Orders\".\"user_id\" "
                  "JOIN \"Cars\" ON \"Orders\".\"car_id\" = \"Cars\".\"id_car\" "
                  "WHERE \"Users\".\"id_user\" = :id_user "
                  "AND \"Orders\".\"end_date\" < :currentDate");
    currentUser.id_user = 1;
    query.bindValue(":id_user", currentUser.id_user);
    query.bindValue(":currentDate", currentDate);

    QVector<QPair<QVector<QString>, QPixmap>> carsList;

    if(query.exec()) {
        while(query.next()) {
            QVector<QString> car;

            car.append(query.value("model").toString());
            car.append(query.value("price").toString());
            car.append(query.value("start_date").toDate().toString("dd MMM"));
            car.append(query.value("end_date").toDate().toString("dd MMM"));

            QPixmap pixmap;
            QByteArray photoData = query.value("photo").toByteArray();

            if(!photoData.isEmpty()) {
                pixmap.loadFromData(photoData);
            }
            carsList.append(qMakePair(car, pixmap));
        }
    }
    create_widgetCard(ui->verticalLayout_18, carsList, false);
}


void MainWindow::create_widgetCard(QVBoxLayout *layoutWidgetCard, QVector<QPair<QVector<QString>, QPixmap>> &carsList, bool isActive)
{
    while(QLayoutItem *item = layoutWidgetCard->takeAt(0)) {
        if(QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    for(int i = 0; i < carsList.size(); ++i) {
        const QVector<QString> &car = carsList[i].first;
        const QPixmap &photoPixmap = carsList[i].second;

        QWidget *widgetCard = new QWidget(this);
        widgetCard->setFixedSize(650, 140);
        layoutWidgetCard->addWidget(widgetCard);

        QGridLayout *layoutCard = new QGridLayout(widgetCard);
        widgetCard->setLayout(layoutCard);
        layoutCard->setContentsMargins(20,20,0,20);
        layoutCard->setHorizontalSpacing(30);

        QLabel *photo = new QLabel(widgetCard);

        photo->setPixmap(roundedPixmap(photoPixmap, 300, 210));
        photo->setFixedSize(150,100);
        photo->setScaledContents(true);

        QLabel *model = new QLabel(car[0], widgetCard);
        model->setStyleSheet("font-weight: 600; font-size: 18px; padding-bottom: 5px;");
        QLabel *dateOfBooking = new QLabel(car[2] + " - " + car[3] + " $" + car[1], widgetCard);
        dateOfBooking->setStyleSheet("padding-bottom: 8px;");
        QLabel *statusOfBooking = new QLabel(widgetCard);

        if (isActive == true) {
            statusOfBooking->setText("Active");
        } else {
            statusOfBooking->setText("Expired");
        }

        layoutCard->addWidget(photo, 0,0, 3, 1);
        layoutCard->addWidget(model, 0,1);
        layoutCard->addWidget(dateOfBooking, 1,1);
        layoutCard->addWidget(statusOfBooking, 2,1);

        widgetCard->setObjectName("widgetCardBookings");
        widgetCard->setStyleSheet("background-color: #1a1a1a;");
    }
    layoutWidgetCard->addStretch();
}

void MainWindow::creationInnerTabs()
{
    refreshData();
    ui->Account_name_EditLine->setText(currentUser.name);
    ui->Account_LastName_EditLine->setText(currentUser.surname);
    ui->Account_phone_EditLine->setText(currentUser.phone);
    ui->Account_email_EditLine->setText(currentUser.email);

    saveChangesButton_personalInformation = new QPushButton(ui->PersonalInformation_tab);
    saveChangesButton_personalInformation->move(0,240);

    saveChangesButton_email = new QPushButton(ui->Email_tab);
    saveChangesButton_email->move(0,160);

    saveChangesButton_changePassword = new QPushButton(ui->ChangePassword_tab);
    saveChangesButton_changePassword->move(0,240);

    for (QPushButton* button : ui->Account_innerTab->findChildren<QPushButton*>()) {
        button->setEnabled(false);
        button->setFixedSize(210,60);
        button->setText("Save");
    }

    connect(saveChangesButton_personalInformation, &QPushButton::clicked, this, &MainWindow::saveChangesButton);
    connect(saveChangesButton_email, &QPushButton::clicked, this, &MainWindow::saveChangesButton);
    connect(saveChangesButton_changePassword, &QPushButton::clicked, this, &MainWindow::saveChangesButton);

    auto checkChangesInformation = [this]() {
        bool changed = (ui->Account_name_EditLine->text() != currentUser.name ||
                        ui->Account_LastName_EditLine->text() != currentUser.surname ||
                        ui->Account_phone_EditLine->text() != currentUser.phone);
        saveChangesButton_personalInformation->setEnabled(changed);
    };

    auto checkChangesEmailPassword = [this]() {
        if(ui->Account_email_EditLine->text() != currentUser.email) {
            saveChangesButton_email->setEnabled(true);
        } else if(ui->Account_currentPassword_EditLine->text() != "" && ui->Account_newPassword_EditLine->text() != "") {
            saveChangesButton_changePassword->setEnabled(true);
        }
    };

    connect(ui->Account_name_EditLine, &QLineEdit::textChanged, this, checkChangesInformation);
    connect(ui->Account_LastName_EditLine, &QLineEdit::textChanged, this, checkChangesInformation);
    connect(ui->Account_phone_EditLine, &QLineEdit::textChanged, this, checkChangesInformation);
    connect(ui->Account_email_EditLine, &QLineEdit::textChanged, this, checkChangesEmailPassword);
    connect(ui->Account_currentPassword_EditLine, &QLineEdit::textChanged, this, checkChangesEmailPassword);
    connect(ui->Account_newPassword_EditLine, &QLineEdit::textChanged, this, checkChangesEmailPassword);

    for(QLineEdit *editLine : ui->Account_innerTab->findChildren<QLineEdit*>()) {
        editLine->installEventFilter(this);
    }

    isSuccessful = new QLabel(ui->Account_innerTab);
    isSuccessful->setObjectName("isSuccessful");
    isSuccessful->setStyleSheet("font-size: 16px; color: green;");
    isSuccessful->hide();
}

void MainWindow::saveChangesButton()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    QSqlQuery query;



    if(ui->Account_innerTab->currentIndex() == 0) {
        query.prepare("UPDATE \"Users\" "
                      "SET \"name\" = :name, \"surname\" = :surname, \"phone\" = :phone "
                      "WHERE \"id_user\" = :id_user");

        query.bindValue(":name", ui->Account_name_EditLine->text());
        query.bindValue(":surname", ui->Account_LastName_EditLine->text());
        query.bindValue(":phone", ui->Account_phone_EditLine->text());
        query.bindValue(":id_user", currentUser.id_user);

        if(query.exec()) {
            isSuccessful->setText("Your data was successfully added");
            isSuccessful->setStyleSheet("font-size: 16px; color: green;");
        } else {
            isSuccessful->setText("Your data wasn't successfully added");
            isSuccessful->setStyleSheet("font-size: 16px; color: red;");
        }
        isSuccessful->move(0,248);
        isSuccessful->show();
        button->setEnabled(false);

        refreshData();
    } else if(ui->Account_innerTab->currentIndex() == 1) {

        query.prepare("UPDATE \"Users\" "
                      "SET \"email\" = :email "
                      "WHERE \"id_user\" = :id_user");

        query.bindValue(":email", ui->Account_email_EditLine->text());
        query.bindValue(":id_user", currentUser.id_user);

        if(query.exec()) {
            isSuccessful->setText("Your data was successfully added");
            isSuccessful->setStyleSheet("font-size: 16px; color: green;");
        } else {
            isSuccessful->setText("Your data wasn't successfully added");
            isSuccessful->setStyleSheet("font-size: 16px; color: red;");
        }
        isSuccessful->move(0,160);
        isSuccessful->show();
        button->setEnabled(false);

        refreshData();
    } else if(ui->Account_innerTab->currentIndex() == 2) {

        if(ui->Account_currentPassword_EditLine->text() == currentUser.password
            && ui->Account_newPassword_EditLine->text() != currentUser.password) {

            query.prepare("UPDATE \"Users\" "
                          "SET \"password\" = :newPassword "
                          "WHERE \"id_user\" = :id_user");

            query.bindValue(":newPassword", ui->Account_newPassword_EditLine->text());
            query.bindValue(":id_user", currentUser.id_user);

            if(query.exec()) {
                isSuccessful->setText("Your data was successfully added");
                isSuccessful->setStyleSheet("font-size: 16px; color: green;");
                ui->Account_currentPassword_EditLine->setText("");
                ui->Account_newPassword_EditLine->setText("");
            } else {
                isSuccessful->setText("Your data wasn't successfully added");
                isSuccessful->setStyleSheet("font-size: 16px; color: red;");
            }
            isSuccessful->move(0,248);
            isSuccessful->show();
            button->setEnabled(false);

            refreshData();
        } else {
            isSuccessful->setText("Something went worng, change your password");
            isSuccessful->setStyleSheet("font-size: 16px; color: red;");
            isSuccessful->move(0,248);
            isSuccessful->show();
        }
    }
}

void MainWindow::refreshData()
{
    currentUser.id_user = 1;
    QSqlQuery query;
    query.prepare("SELECT \"name\", \"surname\", \"phone\", \"email\", \"password\" "
                  "FROM \"Users\" "
                  "WHERE \"id_user\" = :id_user");
    query.bindValue(":id_user", currentUser.id_user);

    if (query.exec()) {
        if (query.next()) {
            currentUser.name = query.value("name").toString();
            currentUser.email = query.value("email").toString();
            currentUser.surname = query.value("surname").toString();
            currentUser.phone = query.value("phone").toString();
            currentUser.password = query.value("password").toString();
        }
    } else {
        qDebug() << "Ошибка запроса: ";
    }
}

void MainWindow::changeCurrentTab()
{
    QLabel* label = ui->Account_innerTab->findChild<QLabel*>("isSuccessful");
    if (label) {
        label->hide();
    }
}

