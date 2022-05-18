#include "loglistview.h"
#include "../dialoglog.h"

LogListView::LogListView(QWidget *parent) : QListView (parent)
{

    QPalette p(palette());

    this->setResizeMode(ResizeMode::Adjust);

    p.setBrush(QPalette::WindowText, QColor("#303030"));
    p.setBrush(QPalette::Base, QColor("#F0F1F2"));
    p.setBrush(QPalette::Light, QColor("#FFFFFF"));
    p.setBrush(QPalette::Midlight, QColor("#D3D6D8"));
    p.setBrush(QPalette::Mid, QColor("#C5C9Cb"));
    p.setBrush(QPalette::Dark, QColor("#9AA0A4"));
    p.setBrush(QPalette::Text, QColor("#616b71"));
    p.setBrush(QPalette::Highlight, QColor("#E2E4E5"));

    setPalette(p);
    setFont(QFont("MSShellDlg 2", 12));
    setFrameStyle(QFrame::NoFrame | QFrame::Plain);

    m_model = new QStandardItemModel(this);
    pMoel = new MyQSortFilterProxyModel(this, this);
    pMoel->setDynamicSortFilter(false);
    pMoel->setSourceModel(m_model);
    pMoel->setFilterKeyColumn(0);
    setModel(pMoel);
    //setModel(m_model);
    setItemDelegate(new LogListItemDelegate(this, true));

    connect(this, &LogListView::clicked, this, &LogListView::on_itemClicked);
    connect(this, &LogListView::doubleClicked, this, [this](const QModelIndex &index){
        expandItem(index);
    });
}

void LogListView::expandItem(const QModelIndex &index)
{
    QStandardItem *item = m_model->itemFromIndex(pMoel->mapToSource(index));
    int height = item->sizeHint().height();
    bool expanded = index.data(LogListItemDelegate::DT_Expanded).toBool();
    if (!expanded) {
        item->setData(true, LogListItemDelegate::DT_Expanded);
        item->setSizeHint(QSize(0, height));
    } else {
        item->setData(false, LogListItemDelegate::DT_Expanded);
        item->setSizeHint(QSize(0, height));
    }
    item->setData(false, LogListItemDelegate::IsNew);
}

void LogListView::on_itemClicked(const QModelIndex &index)
{
    LogListItemDelegate *delegate = static_cast<LogListItemDelegate *>(itemDelegate());
    bool expand = delegate->isExpandable() && mapFromGlobal(QCursor::pos()).x() >= visualRect(index).width() - 48;
    if (expand) {
        expandItem(index);
    }

    // switch between selected and none selected items
    if(lastClickedItem) {
        lastClickedItem->setData(false, LogListItemDelegate::IsSelected);
    }
    QStandardItem *item = m_model->itemFromIndex(pMoel->mapToSource(index));
    item->setData(true, LogListItemDelegate::IsSelected);
    lastClickedItem = item;
}

void LogListView::addMessage(const QJsonObject &msg)
{
    static int id = 1; // unique id for log items
    auto *item = new QStandardItem();
    item->setEditable(false);
    item->setData(QString("%1").arg(id++, 5, 10, QChar('0')), LogListItemDelegate::DT_Id);
    item->setData(msg["icon"], LogListItemDelegate::DT_ICON);
    if(msg.contains("date")) {
        item->setData(msg["date"], LogListItemDelegate::DT_Timestamp);
    } else {
        item->setData(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), LogListItemDelegate::DT_Timestamp);
    }
    item->setData(msg["title"], LogListItemDelegate::DT_Title);
    item->setData(msg["subtitle"], LogListItemDelegate::DT_Subtitle);
    item->setData(msg["msg"], LogListItemDelegate::DT_Details);
    item->setData(false, LogListItemDelegate::DT_Expanded);
    item->setData(true, LogListItemDelegate::IsNew);

    m_model->appendRow(item);
    if(dLog->isScrollToBottom()) {
        scrollToBottom();
    }
}

void LogListView::setDLog(DialogLog *value)
{
    dLog = value;
}

int LogListView::getReadState()
{
    return dLog->getReadState();
}

std::tuple<int, QString> LogListView::getSelectedCamera()
{
    return dLog->getSelectedCamera();
}

int LogListView::getLOgType()
{
    return dLog->getLOgType();
}

QString LogListView::getSerchWord()
{
    return dLog->getSerchWord();
}

void LogListView::invalidateFilter()
{
    pMoel->refresh();
}

void LogListView::clearAll()
{
    m_model->clear();
}

int LogListView::getLogCount()
{
    return m_model->rowCount();
}

//---------------------MyQAbstractProxyModel----------------------------------
MyQSortFilterProxyModel::MyQSortFilterProxyModel(LogListView *llv, QWidget *parent)
    :QSortFilterProxyModel(parent), llv(llv)
{
}

void MyQSortFilterProxyModel::refresh()
{
    invalidateFilter();
}

bool MyQSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    bool result = true;
    int readState = llv->getReadState();
    if(readState > 0) { // if read state was selected
        bool isNew = index.data(LogListItemDelegate::IsNew).toBool();
        result = result&&(readState==1 ? !isNew : isNew); // readState==1 means only read
    }

    auto selectedCamera = llv->getSelectedCamera();
    if(std::get<0>(selectedCamera) > 0) { // if camera was selected
         QString title = index.data(LogListItemDelegate::DT_Title).toString();
         result = result && title.contains(std::get<1>(selectedCamera));
    }

    int logType = llv->getLOgType();
    if(logType > 0) { // if log type was selected
        int iconType = index.data(LogListItemDelegate::DT_ICON).toInt();
        qDebug() << "iconType" << iconType;
        if(logType == 1) {                      // success
            result = result && (iconType == ms::IC_SUCCESS);
        } else if (logType == 2) {              // warn
            result = result && (iconType == ms::IC_INFO);
        } else if (logType == 3) {              // info
            result = result && iconType == ms::IC_WARN;
        } else if (logType == 4) {              // error
            result = result && (iconType == ms::IC_ERROR);
        } else if (logType == 5) {              // alarm
            result = result && iconType == ms::IC_CAMERA_ALARM;
        } else if (logType == 6) {              // stability
            result = result && (iconType == ms::IC_STABILITY_WARN || iconType == ms::IC_STABILITY_ERROR);
        } else if (logType == 7) {              // sys error
            result = result && (iconType == ms::IC_SYS_ALARM || iconType == ms::IC_SYS_ERROR);
        }
    }

    QString searchWord = llv->getSerchWord().trimmed();
    if(!searchWord.isEmpty()) {
        QString title = index.data(LogListItemDelegate::DT_Title).toString();
        QString subtitle = index.data(LogListItemDelegate::DT_Subtitle).toString();
        QString details = index.data(LogListItemDelegate::DT_Details).toString();
        QString text = title+subtitle+details;
        result = result && text.contains(searchWord, Qt::CaseInsensitive);
    }
    return result;
}
