#ifndef COLORLINESREPOSITORY_H
#define COLORLINESREPOSITORY_H

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <qcolor.h>

struct BallData
{
    int row;
    int column;
    bool visible;
    QColor color;
};

class ColorLinesRepository: QObject {
public:
    ColorLinesRepository();

    ~ColorLinesRepository();

    // Метод добавления записи с данными о шарике в таблицу БД
    bool insertRecord(const BallData &data);

    // Метод добавления запись с информацией о баллах таблицу БД
    bool insertScore(int score);

    // Метод обновления записи в таблице БД
    bool updateRecord(int oldRow, int oldColumn, const BallData &data);

    // Метод очистки данных из таблиц
    void clear();

    int getScore();

    std::vector<BallData> getBallsData();

    BallData toBallData(int row, int column, QColor &color, bool visible);

private:
    void openDb();

    // Метод создания таблицы
    void createTable();
};



#endif // COLORLINESREPOSITORY_H
