#include "database.h"
#include "qpixmap.h"

#include <QSqlQuery>
#include <QDebug>
#include <QFile>
#include <QByteArray>
#include <QSqlError>

Database::Database() {

}

Database::~Database() {
    closeDatabase();
}

bool Database::openDatabase() {

    db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("RentalCars");
    db.setUserName("postgres");
    db.setPassword("45456565Maksim_");

    if (!db.open()) {
        qDebug() << "Не удалось открыть базу данных!";
        return false;
    }
    return true;
}

void Database::closeDatabase() {
    db.close();
}

void Database::addPhotoToDatabase()
{
    QFile ImgFile("C:/Users/golov/Downloads/Volkswagen_Arteon.jpg");
    if (!ImgFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Не удалось открыть файл изображения!";
        return;
    }
    QByteArray imageData = ImgFile.readAll();

    ImgFile.close();

    QSqlQuery query;
    query.prepare("UPDATE \"Cars\" SET \"photo\" = :photo WHERE \"id_car\" = :id_car");
    query.bindValue(":photo", imageData);
    query.bindValue(":id_car", 0);

    if (!query.exec()) {
        qDebug() << "Ошибка при вставке данных в базу:" << query.lastError().text();
    } else {
        qDebug() << "Изображение успешно добавлено в базу данных!";
    }
}

QVector<QPair<QVector<QString>, QPixmap>> Database::getCars(QString queryStr)
{
    QVector<QPair<QVector<QString>, QPixmap>> carList;
    QSqlQuery query;

    if(queryStr.isEmpty()) {
        query.prepare("SELECT \"photo\", \"model\", \"specifications\", \"status\", \"fuel\", \"transmission\", \"price\", \"type\", \"passengers\", \"id_car\" FROM \"Cars\"");
    } else {
        query.prepare(queryStr);
    }

    if(query.exec()) {
        while(query.next()) {
            QVector<QString> car;

            car.append(query.value("model").toString());
            car.append(query.value("specifications").toString());
            car.append(query.value("status").toString());
            car.append(query.value("fuel").toString());
            car.append(query.value("transmission").toString());
            car.append(query.value("price").toString());
            car.append(query.value("type").toString());
            car.append(query.value("passengers").toString());
            car.append(query.value("id_car").toString());

            QPixmap pixmap;
            QByteArray photoData = query.value("photo").toByteArray();

            if(!photoData.isEmpty()) {
                pixmap.loadFromData(photoData);
            }

            carList.append(qMakePair(car, pixmap));
        }
    }
    return carList;
}
