#ifndef COLORLINESMODEL_H
#define COLORLINESMODEL_H

#include <QAbstractItemModel>
#include <QColor>

#include <array>
#include <vector>
#include <random>

#include<colorlinesrepository.h>

struct Ball {
    QColor color;
    bool visible{false};
};

enum Role {
    kColorRole = Qt::UserRole,
    kVisibleRole
};

class ColorLinesModel : public QAbstractItemModel {
    Q_OBJECT

    Q_PROPERTY(int columns READ columns CONSTANT)
    Q_PROPERTY(int rows READ rows CONSTANT)
    Q_PROPERTY(int score READ score CONSTANT)
    Q_PROPERTY(int empty_cell READ empty_cell CONSTANT)

public:
    explicit ColorLinesModel(QObject *parent = nullptr);

    // Базовая функциональность для типа Tabl_balleView в QML
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Метод возвращает данные по определенному индексу, где в индексе указан номер строки и номер ячейки
    /*Вызывается при старте чтобы идентифицировать все ячейки в таблице.
     * А также идентифицировать в ячейках таблицы цвета и видимость(т.е показывать цвет в ячейке или нет).
       Цвет и видимость определяется по ролям согласно enum Role.
    */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Метод возвращает имена ролей в виде хеш-таблицы QHash<int, QByteArray>. Чтобы можно было по именам ролей отправлять значения ролей из QML
    QHash<int, QByteArray> roleNames() const override;
    //============================================================================

    static int columns();
    static int rows();
    int score() const;
    int empty_cell() const;

    Q_INVOKABLE void reset();
    Q_INVOKABLE void spawn();
    Q_INVOKABLE void setChosenPosition(int row, int col);
    Q_INVOKABLE void moveElement(int row, int col);
    Q_INVOKABLE void sequenceSearch();
    Q_INVOKABLE bool isChosePosition();

signals:
    void changeScope();
    void gameOver();

private:
    ColorLinesRepository *colorLinesRepository = new ColorLinesRepository();

    // Минимальное количество очков, которые можно получить за правильную последовательность
    static constexpr int min_scores = 10;

    static constexpr int m_columns = 9;
    static constexpr int m_rows = 9;
    static constexpr int m_sequence = 5;
    static constexpr int m_number_colors = 4;
    // Число, которое указывает сколько нужно создать шариков в таблице
    static constexpr int m_number_spawning = 3;

    // Значение строки и столбца указывающие местопложение выбранного элемента
    int m_chosen_row_element{-1};
    int m_chosen_col_element{-1};

    // Баллы
    int m_score{};

    // Кол-во пустых ячеек в таблице
    int m_empty_cell{};

    /* Флаг, который означает, что был вызван метод загрузки данных о шариках для игровой таблицы.
     * Применяется при первом запуске приложения, если есть сохранённое состояние игровой таблицы, т.е расположение шариков на определеленных позициях,
       то чтобы не генерировались дополнительные шарики.
    */
    bool was_balls_data_loaded_invoke{false};

    // Массив с данными таблицы. Где массив внутри массива представляет последотвальность заполненных ячеек шарами
    std::array<std::array<Ball, m_columns>, m_rows> m_board;

    std::vector<QColor> m_colors;

    static std::random_device rd;
    static std::default_random_engine generator;
    static std::uniform_int_distribution<int> distribution_cell;
    static std::uniform_int_distribution<int> distribution_color;

    bool isEmptyCell(int row, int col);

    // Метод для поиска последовательности, сплошной линии шаров одного цвета
    void baseSequenceSearch(std::vector<std::pair<int, int>> &vector_row_column, QColor &color, int row, int column);

    void rowSequenceSearch();
    void columnSequenceSearch();

    // Метод для подтверждения, что последовательность является сплошной линий из шаров одного цвета
    bool isSequence(std::vector<std::pair<int, int>> &vector_row_column);

    // Метод загрузки данных о шариках в табилцу. Например, из БД
    void loadBallsData();
};

#endif // COLORLINESMODEL_H
