#include "loglistitemdelegate.h"

LogListItemDelegate::LogListItemDelegate(QObject *parent, bool expandable) :
    QStyledItemDelegate(parent),
    m_expandable(expandable)
{

}

bool LogListItemDelegate::isExpandable() const
{
    return m_expandable;
}

void LogListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    int iconType = index.data(DT_ICON).toInt();
    QString icon;
    if(iconType == ms::IC_SUCCESS) {        // success
        icon = ":/icons/seccess.png";
    } else if(iconType == ms::IC_WARN) {    // warning
        icon = ":/icons/status-warn.png";
    } else if(iconType == ms::IC_ERROR) {   // error
        icon = ":/icons/status-error.png";
    } else if(iconType == ms::IC_CAMERA_ALARM) {    // detection alarm
        icon = ":/icons/alarm_2.png";
    } else if(iconType == ms::IC_SYS_ALARM) {       // system alarm due to camera alarm
        icon = ":/icons/system_err32x32.png";
    } else if(iconType == ms::IC_SYS_ERROR) {       // system error due to software image process error, net disconnection or camera behavior problem ...etc
        icon = ":/icons/v_brightness.png";
    } else if(iconType == ms::IC_STABILITY_WARN) {  // camera stability warning
        icon = ":/icons/cam_stability_warn.png";
    } else if(iconType == ms::IC_STABILITY_ERROR) { // camera stability warning
        icon = ":/icons/cam_stability_err.png";
    } else if(iconType == ms::IC_INFO) {            // info log message
        icon = ":/icons/info_32x32.png";
    }

    QString id = index.data(DT_Id).toString();
    QString timestamp = index.data(DT_Timestamp).toString();
    QString title = index.data(DT_Title).toString();
    QString subtitle = index.data(DT_Subtitle).toString();

    bool hovered = opt.state & QStyle::State_MouseOver;
    bool expanded = index.data(DT_Expanded).toBool();
    bool isSelected = index.data(IsSelected).toBool();

    QFont f(opt.font);
    f.setPointSize(10);

    painter->save();

    // draw hover
    if(hovered && !isSelected) {
        painter->fillRect(opt.rect.adjusted(0, 0, 0, 0), "#FAFAFA");
    } else if(isSelected) {
        painter->fillRect(opt.rect.adjusted(0, 0, 0, 0), "#F8ECE0");
    } else {
        painter->fillRect(opt.rect.adjusted(0, 0, 0, 0), "#FFFFFF");
    }

    // Draw message icon
    painter->drawPixmap(8, opt.rect.y() + 16, QIcon(icon).pixmap(32, 32));

    // Draw id
    painter->setFont(f);
    painter->setPen(opt.palette.text().color());
    painter->drawText(opt.rect.adjusted(48, 8, 0, 0), Qt::TextSingleLine, id);

    // draw the icon which indicates this message was read
    bool isNew = index.data(IsNew).toBool();
    if(isNew) {
        //start_record
        painter->drawPixmap(8+32+48+5, opt.rect.y()+8, QIcon(":/icons/remove_minus_sign_small.png").pixmap(32, 32));
    }

    // Draw timestamp
    painter->setFont(f);
    int wTimestamp = painter->fontMetrics().boundingRect(timestamp).width();
    painter->setPen(opt.palette.text().color());
    painter->drawText(opt.rect.adjusted((opt.rect.width()/2)-(wTimestamp/2), 8, 0, 0), Qt::TextSingleLine, timestamp);

    // Draw title
    QFont fntTitle(opt.font);
    fntTitle.setBold(true);
    fntTitle.setPointSize(12);
    painter->setFont(fntTitle);
    painter->setPen(opt.palette.windowText().color());
    painter->drawText(opt.rect.adjusted(48, 32, 0, 0), Qt::TextSingleLine, title);

    // Draw subtitle
    int wTitle = painter->fontMetrics().boundingRect(title).width();
    QFont fntSubTitle(opt.font);
    fntTitle.setPointSize(10);
    painter->setFont(fntSubTitle);
    if(iconType == ms::IC_ERROR) {
        painter->setPen(QColor(255, 0, 0));
    } else {
        painter->setPen(opt.palette.windowText().color());
    }
    painter->drawText(opt.rect.adjusted(48+wTitle+12, 32, 0, 0), Qt::TextSingleLine, subtitle);

    //expand icon
    QString details = index.data(DT_Details).toString();
    if (expanded) {
        QString details = index.data(DT_Details).toString();

        QFont fntDetials(opt.font);
        painter->setFont(fntDetials);
        fntDetials.setPointSize(12);
        if(iconType == ms::IC_ERROR) {
            painter->setPen(QColor(255, 0, 0));
        } else {
            painter->setPen(opt.palette.windowText().color());
        }

        painter->drawText(opt.rect.adjusted(48, 32+32, 0, 0), Qt::TextWordWrap, details);
        painter->drawPixmap(opt.rect.width() - 31, opt.rect.y()+24, QPixmap(":/icons/up.png"));
    } else {
        painter->setOpacity(1);
        painter->drawPixmap(opt.rect.width() - 31, opt.rect.y()+24, QPixmap(":/icons/down.png"));
    }

    // draw button line
    bool lastIndex = (index.model()->rowCount() - 1) == index.row();
    int bottomEdge = opt.rect.bottom();
    painter->setPen(lastIndex ? QColor("#000000") : QColor("#E6E6E6"));
    painter->drawLine(lastIndex ? 0 : 48, bottomEdge, opt.rect.right(), bottomEdge);

    painter->restore();
}

QSize LogListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool expanded = index.data(DT_Expanded).toBool();
    if(expanded) {
        QFont fntDetials = option.font;
        fntDetials.setPointSize(12);
        QRect r = option.rect;
        QFontMetrics fm(fntDetials);
        QString details = index.data(DT_Details).toString();
        QRect br = fm.boundingRect(r, Qt::TextWordWrap, details);
        return QSize(option.rect.width(), br.height()+64+30);
    }
    return QSize(option.rect.width(), 64);
}

