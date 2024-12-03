#include "colorlinesmodel.h"

std::random_device ColorLinesModel::rd;
std::default_random_engine ColorLinesModel::generator(rd());
std::uniform_int_distribution<int> ColorLinesModel::distribution_cell(0, m_rows - 1);
std::uniform_int_distribution<int> ColorLinesModel::distribution_color(0, 4);

ColorLinesModel::ColorLinesModel(QObject *parent)
    : QAbstractItemModel(parent), m_empty_cell{m_columns * m_rows} {
    m_colors.push_back(QColor(140, 94, 78));
    m_colors.push_back(QColor(94, 146, 48));
    m_colors.push_back(QColor(49, 48, 74));
    m_colors.push_back(QColor(139, 34, 95));
    m_colors.push_back(QColor(100, 89, 28));

    loadBallsData();
}

void ColorLinesModel::loadBallsData() {
    std::vector<BallData> ballsData = colorLinesRepository->getBallsData();

    for (BallData ballData: ballsData) {
        Ball ball;

        ball.color = ballData.color;
        ball.visible = ballData.visible;

        m_board[ballData.row][ballData.column] = ball;

        was_balls_data_loaded_invoke = true;
    }

    for (auto &row : m_board) {
        for (auto &cell : row) {
            if (cell.visible == true) {
                --m_empty_cell;
            }
        }
    }
}

QModelIndex ColorLinesModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  return createIndex(row, column);
}

QModelIndex ColorLinesModel::parent([[maybe_unused]] const QModelIndex &index) const {
  return {};
}

int ColorLinesModel::rowCount([[maybe_unused]] const QModelIndex &parent) const {
  return m_columns;
}

int ColorLinesModel::columnCount([[maybe_unused]] const QModelIndex &parent) const {
  return m_rows;
}

QVariant ColorLinesModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == kColorRole) {
        return m_board[index.row()][index.column()].color;
    } else if (role == kVisibleRole) {
        return m_board[index.row()][index.column()].visible;
    }

    return QVariant();
}

bool ColorLinesModel::setData(const QModelIndex &index, const QVariant &value, int role) { 
  if (data(index, role) != value && index.isValid()) {
    if (role == kColorRole) {
      m_board[index.row()][index.column()].color = value.value<QColor>();
    } else if (role == kVisibleRole) {
      m_board[index.row()][index.column()].visible = value.value<bool>();
    }

    emit dataChanged(index, index, {role});
    return true;
  }
  return false;
}

Qt::ItemFlags ColorLinesModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;

  return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> ColorLinesModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[kColorRole] = "color";
  roles[kVisibleRole] = "visible";
  return roles;
}

int ColorLinesModel::columns() { return m_columns; }

int ColorLinesModel::rows() { return m_rows; }

int ColorLinesModel::score() const {
    return m_score;
}

void ColorLinesModel::reset() {
    qDebug()<<"Reset table with balls";

    m_score = 0;
    for (auto &row : m_board) {
        for (auto &cell : row) {
            cell.visible = false;
        }
    }
    m_empty_cell = m_rows * m_columns;

    colorLinesRepository->clear();
    was_balls_data_loaded_invoke = false;

    emit dataChanged(index(0, 0), index(m_rows - 1, m_columns - 1));
}

void ColorLinesModel::spawn() {    
    if (!was_balls_data_loaded_invoke) {
        qDebug() << "Create balls in random positions with random colors";

        int row{};
        int col{};
        for (int spawn = 0; spawn < m_number_spawning; ++spawn) {
            do {
                row = distribution_cell(generator);
                col = distribution_cell(generator);
            } while (!isEmptyCell(row, col));

            QColor color = m_colors[distribution_color(generator)];

            m_board[row][col].color = color;
            m_board[row][col].visible = true;

            colorLinesRepository->insertRecord(
                colorLinesRepository->toBallData(row, col, color, true));

            --m_empty_cell;
            if (m_empty_cell < m_number_spawning) {
                emit gameOver();
            }

            emit dataChanged(index(row, col), index(row, col));
        }
    } else {
        // При первом запуске загружаем сохранённые баллы, которые были в последней раз в запущенном приложении
        m_score = colorLinesRepository->getScore();

        emit changeScope();
    }
}

void ColorLinesModel::setChosenPosition(int row, int col) {
    qDebug() << "Chosen element with row number: " << row <<" and column number: " << col;

    m_chosen_row_element = row;
    m_chosen_col_element = col;
}

void ColorLinesModel::moveElement(int row, int col) {
    if (m_chosen_row_element != -1 || m_chosen_col_element != -1) {
        qDebug() << "Move element to row number: " << row << " and column number: " << col;

        // Отключаем флаг, чтобы шарики вновь могли генерироваться случайным образом
        was_balls_data_loaded_invoke = false;

        m_board[m_chosen_row_element][m_chosen_col_element].visible = false;

        emit dataChanged(index(m_chosen_row_element, m_chosen_col_element), index(m_chosen_row_element, m_chosen_col_element));

        QColor color = m_board[m_chosen_row_element][m_chosen_col_element].color;
        m_board[row][col].color = color;

        colorLinesRepository->updateRecord(m_chosen_row_element,
                                           m_chosen_col_element,
                                           colorLinesRepository->toBallData(row, col, color, true));

        m_chosen_row_element = row;
        m_chosen_col_element = col;

        m_board[m_chosen_row_element][m_chosen_col_element].visible = true;

        emit dataChanged(index(m_chosen_row_element, m_chosen_col_element), index(m_chosen_row_element, m_chosen_col_element));

        m_chosen_row_element = -1;
        m_chosen_col_element = -1;
    }
}

void ColorLinesModel::sequenceSearch() {
    rowSequenceSearch();
    columnSequenceSearch();

    emit dataChanged(index(0, 0), index(m_rows - 1, m_columns - 1));
}

bool ColorLinesModel::isChosePosition() {
    return m_chosen_row_element >= 0 && m_chosen_col_element >= 0;
}

int ColorLinesModel::empty_cell() const {
    return m_empty_cell;
}

bool ColorLinesModel::isEmptyCell(int row, int col) {
    return !m_board[row][col].visible;
}

void ColorLinesModel::columnSequenceSearch() {
    /* Контейнер шаров в виде вектора, в которм формируется последовательность шаров как единая линия одного цвета.
     * Количестов шаров добавленных шаров в вектор контролируется через переменную m_sequence
    */
    std::vector<std::pair<int, int>> vector_row_column;

    QColor color{};

    bool flag{false};

    int index_column = 0;
    // В данном цикле ищем линии только по вертикали
    //Указан m_board[0] потому что кол-во столбцов может отличаться от кол-ва строк
    while (index_column < m_board[0].size()) {
        for (int i = 0; i < m_board.size(); ++i) {
            baseSequenceSearch(vector_row_column, color, i, index_column);
        }

        ++index_column;
    }
}

void ColorLinesModel::rowSequenceSearch() {
    /* Контейнер шаров в виде вектора, в которм формируется последовательность шаров как единая линия одного цвета.
     * Количестов шаров добавленных шаров в вектор контролируется через переменную m_sequence
    */
    std::vector<std::pair<int, int>> vector_row_column;

    QColor color{};

    bool flag{false};

    // В данном цикле ищем линии только по горизонтали
    int index_row = 0;
    while (index_row < m_board.size()) {
        for (int j = 0; j < m_board[index_row].size(); ++j) {
            baseSequenceSearch(vector_row_column, color, index_row, j);
        }

        ++index_row;
    }
}

bool ColorLinesModel::isSequence(std::vector<std::pair<int, int>> &vector_row_column) {
    if (vector_row_column.size() >= m_sequence) {
        // Переменная, в которую складывается число шаров одного цвета сплошной горизонтальной линии
        int horizontally = 0;

        // Переменная, в которую складывается число шаров одного цвета сплошной вертикальной линии
        int vertically = 0;

        /* В данном цикле проверяется чтобы в векторе была сплошная линия из шаров одного цвета, т.е чтобы не было разрывов между шарами */
        for (int index_vector = 0; index_vector <= vector_row_column.size(); ++index_vector) {
            if (index_vector == vector_row_column.size() && (vector_row_column[index_vector-1].second - vector_row_column[index_vector-2].second == 1)) {
                ++horizontally;
                break;
            }

            if (vector_row_column[index_vector+1].second - vector_row_column[index_vector].second == 1) {
                ++horizontally;
            }

            if (index_vector == vector_row_column.size() && (vector_row_column[index_vector-1].first - vector_row_column[index_vector-2].first == 1)) {
                ++vertically;
                break;
            }

            if (vector_row_column[index_vector+1].first - vector_row_column[index_vector].first == 1) {
                ++vertically;
            }
        }

        if (horizontally >= m_sequence || vertically >= m_sequence) {
            for (int index_vector = 0; index_vector < vector_row_column.size(); ++index_vector) {
                m_board[vector_row_column[index_vector].first][vector_row_column[index_vector].second].visible = false;
            }

            // Начисляем баллы
            m_score += min_scores;

            m_empty_cell += horizontally >= m_sequence ? horizontally : vertically;

            vector_row_column.clear();

            return true;
        }
    }

    return false;
}

void ColorLinesModel::baseSequenceSearch(std::vector<std::pair<int, int>> &vector_row_column, QColor &color, int row, int column) {
    bool flag{false};

    std::pair<int,int> row_column;

    if (m_board[row][column].visible) {
        if (vector_row_column.empty()) {
            row_column.first = row;
            row_column.second = column;

            vector_row_column.emplace_back(row_column);

            color = m_board[row][column].color;

            qDebug() << "Added a ball with row number: " << row << " and column number: " << column
                     << " of color: red " << qRed(color.rgb()) << ", green "<< qGreen(color.rgb()) << ", blue " << qBlue(color.rgb()) <<
                " to the sequence (vector type)";

        } else if (m_board[row][column].color == color) {
            qDebug() << "Added a ball with row number: " << row << " and column number: " << column
                     << " of color: red "<<qRed(color.rgb())<<", green "<<qGreen(color.rgb())<<", blue "<<qBlue(color.rgb())<<
                " to the sequence (vector type)";

            row_column.first = row;
            row_column.second = column;

            vector_row_column.emplace_back(row_column);

            flag = isSequence(vector_row_column);
        } else {
            flag = isSequence(vector_row_column);

            if (!vector_row_column.empty()) {
                qDebug() << "Clear vector_row_column vector";
                vector_row_column.clear();
            }

            color = m_board[row][column].color;

            qDebug() << "Added a ball with row number: " << row << " and column number: " << column
                     << " of color: red " << qRed(color.rgb()) << ", green " << qGreen(color.rgb()) << ", blue " << qBlue(color.rgb()) <<
                " to the sequence (vector type)";

            row_column.first = row;
            row_column.second = column;

            vector_row_column.emplace_back(row_column);
        }

    } else {
        flag = isSequence(vector_row_column);
    }

    if (flag) {
        colorLinesRepository->insertScore(m_score);

        emit changeScope();
    }
}
