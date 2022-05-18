#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include <QListView>
#include <QStandardItemModel>
#include <QMouseEvent>
#include <QDateTime>
#include <QVariantAnimation>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSortFilterProxyModel>
#include "loglistitemdelegate.h"
#include "systembroadcast.h"

class LogListView;

class MyQSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    MyQSortFilterProxyModel(LogListView *llv, QWidget *parent=nullptr);

    void refresh();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    LogListView *llv;
};

class LogListView : public QListView
{
    Q_OBJECT

public:
    LogListView(QWidget *parent=nullptr);

    void setDLog(DialogLog *value);

    int getReadState();
    std::tuple<int, QString> getSelectedCamera();
    int getLOgType();
    QString getSerchWord();

    void invalidateFilter();
    void clearAll();
    int getLogCount();

private slots:
    void on_itemClicked(const QModelIndex &index);

public slots:
    void addMessage(const QJsonObject &msg);

signals:
    void itemSelected(int);

private:
    const int m_heightHeader = 64;
    QStandardItemModel *m_model;
    MyQSortFilterProxyModel *pMoel;
    DialogLog *dLog;

    QStandardItem *lastClickedItem=nullptr;

    QFile *logFile;

    void expandItem(const QModelIndex &index);
};

#endif // LOGLISTVIEW_H
