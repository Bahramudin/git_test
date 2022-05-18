#ifndef LOGLISTITEMDELEGATE_H
#define LOGLISTITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QTextLayout>
#include <QDebug>
#include "../dialoglog.h"

class LogListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum DataType
    {
        DT_Id = Qt::UserRole,
        DT_ICON,
        DT_Timestamp,
        DT_Title,
        DT_Subtitle,
        DT_Details,
        DT_Pressed,
        DT_Expanded,
        DT_Offset,
        IsNew,
        IsSelected
    };

    explicit LogListItemDelegate(QObject *parent, bool expandable);

    bool isCheckable() const;
    bool isExpandable() const;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const;

private:
    bool m_expandable;
};

#endif // LOGLISTITEMDELEGATE_H
