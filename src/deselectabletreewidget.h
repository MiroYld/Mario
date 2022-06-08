//From https://stackoverflow.com/questions/2761284/is-it-possible-to-deselect-in-a-qtreeview-by-clicking-off-an-item
#ifndef DESELECTABLETREEWIDGET_H
#define DESELECTABLETREEWIDGET_H

#include "QTreeWidget"
#include "QMouseEvent"

class DeselectableTreeWidget : public QTreeWidget{
public:
    DeselectableTreeWidget(QWidget *parent) : QTreeWidget(parent) {}
    virtual ~DeselectableTreeWidget() {}

private:
    virtual void mousePressEvent(QMouseEvent *event){
        QModelIndex item = indexAt(event->pos());
        bool selected = selectionModel()->isSelected(indexAt(event->pos()));
        QTreeWidget::mousePressEvent(event);
        if ((item.row() == -1 && item.column() == -1) || selected){
            clearSelection();
            const QModelIndex index;
            selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
        }
    }
};

#endif // DESELECTABLETREEWIDGET_H
