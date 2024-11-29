import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CLModel 1.0

TableView {
  id: _tv

  property alias _tv_model: _clm

  // Расстояние между строками
  rowSpacing: 5

  // Расстояние между столбцами
  columnSpacing: rowSpacing

  // Данный делегат описывает шаблон отображения данных каждого элемента ячейки
  delegate: Rectangle {
    id: cell

    // Неявное значение ширины
    implicitWidth: (_tv.width - _tv.columnSpacing * (_clm.columns - 1)) / _clm.columns
    // Неявное значение высоты
    implicitHeight: (_tv.height - _tv.rowSpacing * (_clm.rows - 1)) / _clm.rows

    radius: implicitWidth * 0.1

    MouseArea {
      id: _ma_cell
      anchors.fill: parent

      onClicked: {
        if (!_ball.visible && _clm.isChosePosition()) {
          _clm.moveElement(row, column)
          _clm.spawn()
          _clm.sequenceSearch()
        }
      }
    }

    // Ещё одна прямоугольник в ячейке (в прямоугольнике с id=сell) для преобразования в шар
    Rectangle {
      id: _ball

      anchors.centerIn: parent

      visible: model.visible

      width: cell.width * 0.9
      height: cell.height * 0.9

      color: model.color

      // Превращение прямоугольника в шар за счёт изменения радиуса
      radius: width * 0.5

      ScaleAnimator {
        id: _sa_ball
        target: _ball
        from: 0.9
        to: 1
        duration: 1000
      }

      MouseArea {
        id: _ma_ball
        anchors.fill: parent

        //Обрабатывается событие мыши, когда нажимаем на шар в таблице. Где row - номер строки в таблице, column - номер столбца в таблице
        onClicked: {
          if (_ball.visible) {
            _sa_ball.running = true
            _clm.setChosenPosition(row, column)
          }
        }
      }
    }
  }

  model: ColorLinesModel {
    id: _clm
  }

  Component.onCompleted: {
    _clm.spawn()
  }
}
