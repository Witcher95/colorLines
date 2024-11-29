#include "colorlinesrepository.h"
#include <qcolor.h>

ColorLinesRepository::ColorLinesRepository() {
    openDb();
}

void ColorLinesRepository::closeDb() {
    db.close();
}

bool ColorLinesRepository::openDb() {
    // Подключение к базе данных SQLite
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("mydatabase.db");
    if (!db.open()) {
        qDebug() << "Database connection error: " << db.lastError().text();
        return false;
    } else {
        qDebug() << "Database connection successfully!";

        createTable();

        return true;
    }
}

void ColorLinesRepository::createTable() {
    // Создание таблицы с данными шариков с игровой таблицы
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS balls ("
               "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "  row_number INTEGER NOT NULL,"
               "  column_number INTEGER NOT NULL,"
               "  visible BOOL NOT NULL,"
                " red VARCHAR(20) NOT NULL CHECK(red !=''),"
                " green VARCHAR(20) NOT NULL CHECK(green !=''),"
                " blue VARCHAR(20) NOT NULL CHECK(blue !='')"
                    ");")) {
        qDebug() << "DataBase: error of create " << "balls";
        qDebug() << query.lastError().text();
    } else {
        qDebug() << "DataBase: create table " << "balls " << "successfully!";
    }

    // Создание таблицы с баллами
    if (!query.exec("CREATE TABLE IF NOT EXISTS score ("
                    "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "  score INTEGER NOT NULL"
                    ");")) {
        qDebug() << "DataBase: error of create " << "score";
        qDebug() << query.lastError().text();
    } else {
        qDebug() << "DataBase: create table " << "score " << "successfully!";
    }
}

bool ColorLinesRepository::insertRecord(const BallData &data) {
    QSqlQuery query;

    int red = qRed(data.color.rgb());
    int green = qGreen(data.color.rgb());
    int blue = qBlue(data.color.rgb());
    qDebug() << "Insert record about ball: row "<< data.row <<
            ", column " << data.column <<
            ", red " << red <<
            ", green " << green <<
            ", blue " << blue <<
            ", visible " << data.visible <<
            "...";

    query.prepare("INSERT INTO balls(row_number, column_number, visible, red, green, blue)" \
                  "VALUES(:row_number, :column_number, :visible, :red, :green, :blue)");
    query.bindValue(":row_number", data.row);
    query.bindValue(":column_number", data.column);
    query.bindValue(":visible", data.visible);
    query.bindValue(":red", red);
    query.bindValue(":green", green);
    query.bindValue(":blue", blue);
    if(!query.exec())
    {
        qDebug() << "insertRecord error: " << query.lastError();
        return false;
    } else {
        qDebug() << "insertRecord successfully!";
    }

    return true;
}

bool ColorLinesRepository::insertScore(int score) {
    QSqlQuery query;

    qDebug() << "Insert record about score: "<< score << "...";

    query.prepare("INSERT INTO score(score)" \
                  "VALUES(:score)");
    query.bindValue(":score", score);

    if(!query.exec())
    {
        qDebug() << "insertScore error: " << query.lastError();
        return false;
    } else {
        qDebug() << "insertScore successfully!";
    }

    return true;
}

bool ColorLinesRepository::updateRecord(int oldRow, int oldColumn, const BallData &data) {
    QSqlQuery query;

    qDebug() << "Update record about ball: oldRow " << oldRow << ", oldColumn " << oldColumn << "...";

    query.prepare("UPDATE balls SET row_number = :row_number, column_number = :column_number, visible = :visible, red = :red, green = :green, blue = :blue " \
                  "WHERE row_number = :old_row AND column_number = :old_column");
    query.bindValue("old_row", oldRow);
    query.bindValue("old_column", oldColumn);
    query.bindValue(":row_number", data.row);
    query.bindValue(":column_number", data.column);
    query.bindValue(":visible", data.visible);
    query.bindValue(":red", qRed(data.color.rgb()));
    query.bindValue(":green", qGreen(data.color.rgb()));
    query.bindValue(":blue", qBlue(data.color.rgb()));
    if(!query.exec())
    {
        qDebug() << "updateRecord error: " << query.lastError();
        return false;
    } else {
        qDebug() << "updateRecord successfully!";
    }

    return true;
}

void ColorLinesRepository::clear() {
    QSqlQuery query;

    query.exec("DELETE FROM balls");
    query.exec("DELETE FROM score");
}

int ColorLinesRepository::getScore() {
    QSqlQuery query;

    query.exec("SELECT max(score) FROM score");
    while (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

std::vector<BallData> ColorLinesRepository::getBallsData() {
    QSqlQuery query;

    std::vector<BallData> ballsData;

    query.exec("SELECT row_number, column_number, visible, red, green, blue FROM balls");
    while (query.next()) {
        BallData ballData;

        QColor *color = new QColor(query.value(3).toInt(), query.value(4).toInt(), query.value(5).toInt());

        ballData.row = query.value(0).toInt();;
        ballData.column = query.value(1).toInt();
        ballData.color = *color;
        ballData.visible = query.value(2).toBool();

        ballsData.emplace_back(ballData);
    }

    return ballsData;
}

BallData ColorLinesRepository::toBallData(int row, int column, QColor &color, bool visible) {
    BallData ballData;

    ballData.row = row;
    ballData.column = column;
    ballData.color = color;
    ballData.visible = visible;

    return ballData;
}
