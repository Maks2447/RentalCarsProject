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

    this->setObjectName("this");
    ui->LoginButton->setObjectName("LoginButton");
    container->setObjectName("container");
    ui->setFilters_pushButton->setObjectName("setFilters");

    ui->OrderHome->setObjectName("OrderPage");

    connect(ui->backToHomePage, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    loadCars("");

    //setData(user);

    applyStyleSheet();

    QFrame *line = new QFrame(this);
    line->setFixedHeight(1);
    line->setStyleSheet("background-color: gray;");

    QVBoxLayout *layoutLine = new QVBoxLayout();
    ui->widget_8->setLayout(layoutLine);
    layoutLine->addWidget(line);   
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadCars(QString queryStr)
{

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
            orderCarShow(car);
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
}

void MainWindow::setData(const UserData &user)
{
    currentUser = user;
    isLogin = true;

    double loginButtonWidth;

    QString UserName = currentUser.name + " " +currentUser.surname;
    //QString UserName = "Holoviznyi Maksym";
    QFontMetrics fm(ui->LoginButton->font()); // Берёт шрифт кнопки
    int textWidth = fm.horizontalAdvance(UserName);

    loginButtonWidth = textWidth + 40;

    ui->LoginButton->setMinimumWidth(loginButtonWidth);
    ui->LoginButton->setMaximumWidth(loginButtonWidth);

    ui->LoginButton->setText(UserName);

    qDebug() << loginButtonWidth;

    QMenu *menu = new QMenu(this);

    QAction *account = menu->addAction("Account");
    account->setIcon(QIcon("C:/Users/golov/Downloads/icons8-руль-24.png"));

    QAction *bookings = menu->addAction("Bookings");
    bookings->setIcon(QIcon("C:/Users/golov/Downloads/exclamationmark_white.png"));

    menu->addSeparator();
    QAction *logout = menu->addAction("logout");

    ui->LoginButton->setMenu(menu);


    connect(account, &QAction::triggered, this, [=](){
        QMessageBox::information(this, "Аккаунт", "Переход в аккаунт");
    });

    connect(bookings, &QAction::triggered, this, [=](){
        QMessageBox::information(this, "Заказы", "Показать заказы");
    });

    connect(logout, &QAction::triggered, this, [=](){
        QMessageBox::information(this, "Заказы", "Показать заказы");
    });
    menu->setMinimumWidth(loginButtonWidth);
    menu->setStyleSheet(
        "QMenu {"
        "   background-color: #212121;"
        "   color: white;"
        "   margin: 0; "
        "   padding: 0; "
        "}"

        "QMenu::icon {"
        "   padding-left: 20px;"
        "}"

        "QMenu::item {"
        "   height: 30px;"
        "   padding: 7px 10px;"
        "   font-size: 18px;"
        "   padding-left: 15px;"
        "}"

        "QMenu::item:selected {"
        "   background-color: #1c1c1c;"
        "   color: white;"
        "}"

        "QMenu::separator {"
        "   height: 2px;"
        "   background: #444;"
        "   margin: 5px 0;"
        "}"
        );

    //ui->stackedWidget->setCurrentWidget(ui->page_2);
    //connect(ui->LoginButton, &QPushButton::clicked(), this, )
}

void MainWindow::applyStyleSheet()
{
    QFile file(":/mainStyle.css");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
}

void MainWindow::orderCarShow(const QVector<QString>& carData)
{
    ui->stackedWidget->setCurrentWidget(ui->OrderHome);
    qDebug() << currentUser.name;
    if(isLogin) {
        ui->lineEdit->setText(currentUser.name);
        ui->lineEdit_2->setText(currentUser.surname);
    }
    ui->label_11->setText(carData[0]);
    ui->label_12->setText(carData[5]);
}

void MainWindow::on_LoginButton_clicked()
{
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
    clearCars();
    loadCars(getFilters());
    layout->addStretch();
}


