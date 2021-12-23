/*
***************************************************************************************************
* ���������
* ���������Դ�롢�����ƣ�Ϊ��Դ�������ѭMIT��Դ���Э�飨������������MIT���Э�鷢�ͳ�ͻ���Ա�����Ϊ׼����
* �κθ��˻���֯�����Բ������Ƶ�ʹ�á��޸ĸ������
* �κθ��˻���֯��������Դ����Ͷ����Ƶ���ʽ���·ַ���ʹ�ø������
* �κθ��˻���֯�����Բ������Ƶؽ�����������޸ĺ������汾��������ҵ������ҵ����;��
* 
* ����������
* ���߲���ʹ�á��޸ġ��ַ����Լ������κ���ʽ��ʹ�ã���������������޸ĺ������汾���������ĺ�������κη������Ρ�
* ����Ҳ���Ը�����İ�ȫ�ԡ��ȶ��������κα�֤��
* 
* ʹ�á��޸ġ��ַ������ʱ��Ҫ����������������Ĭ���Ѿ�ͬ������������
***************************************************************************************************
*/
#ifndef FRAMELESS_WINDOW_HPP_
#define FRAMELESS_WINDOW_HPP_
#pragma once

#include <QString>
#include <QFile>
#include <QWidget>
#include <QScreen>
#include <QMouseEvent>
#include <QApplication>

template <typename T>
class FramelessWindow : public T {
   public:
    enum class Direction {
        UP = 0,
        DOWN = 1,
        LEFT,
        RIGHT,
        LEFTTOP,
        LEFTBOTTOM,
        RIGHTBOTTOM,
        RIGHTTOP,
        NONE
    };

    FramelessWindow::FramelessWindow(bool translucentBackground, QWidget* parent = Q_NULLPTR) :
        T(parent), m_bLeftPressed(false), m_bResizeable(false), m_bUseSystemMove(false), m_Direction(Direction::NONE), m_iResizeRegionPadding(4) {
        m_iResizeRegionPadding = m_iResizeRegionPadding * this->devicePixelRatioF();
        installEventFilter(this);

        Qt::WindowFlags flags = windowFlags();
        setWindowFlags(flags | Qt::FramelessWindowHint);

        if (translucentBackground) {
            setAttribute(Qt::WA_TranslucentBackground);
        }
    }

    FramelessWindow::~FramelessWindow() {}

    void setTitlebar(QVector<QWidget*> titleBar) { m_titlebarWidget = titleBar; }

    void setResizeable(bool b) { m_bResizeable = b; }

    bool resizeable() const { return m_bResizeable; }

    void setAllWidgetMouseTracking(QWidget* widget) {
        if (!widget)
            return;
        widget->setMouseTracking(true);
        QObjectList list = widget->children();
        foreach (QObject* obj, list) {
            if (obj->metaObject()->className() == QStringLiteral("QWidget")) {
                QWidget* w = (QWidget*)obj;
                w->setMouseTracking(true);
                setAllWidgetMouseTracking(w);
            }
        }
    }

   protected:
    bool eventFilter(QObject* target, QEvent* event) {
        if (event->type() == QEvent::Paint) {
            static bool first = false;
            if (!false) {
                first = true;
                setAllWidgetMouseTracking(this);
            }
        }
        return T::eventFilter(target, event);
    }

    void mouseDoubleClickEvent(QMouseEvent* event) {
        if (m_bResizeable) {
            QWidget* actionWidget = QApplication::widgetAt(event->globalPos());
            if (actionWidget) {
                bool inTitlebar = false;
                for (auto& item : m_titlebarWidget) {
                    if (actionWidget == item) {
                        inTitlebar = true;
                        break;
                    }
                }
                if (inTitlebar) {
                    if (this->isMaximized()) {
                        this->showNormal();
                    }
                    else {
                        this->showMaximized();
                    }
                }
            }
        }
        T::mouseDoubleClickEvent(event);
    }

    void mousePressEvent(QMouseEvent* event) {
        if (event->button() == Qt::LeftButton) {
            if (m_Direction != Direction::NONE) {
                m_bLeftPressed = true;
                this->mouseGrabber();
            }
            else {
                QWidget* action = QApplication::widgetAt(event->globalPos());
                if (action) {
                    bool inTitlebar = false;
                    for (auto& item : m_titlebarWidget) {
                        if (action == item) {
                            inTitlebar = true;
                            break;
                        }
                    }
                    if (inTitlebar) {
                        m_bLeftPressed = true;
                        m_DragPos = event->globalPos() - this->frameGeometry().topLeft();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
                        m_bUseSystemMove = this->windowHandle()->startSystemMove();
                        Q_ASSERT_X(m_bUseSystemMove, "mousePressEvent()", "this->windowHandle()->startSystemMove() failed");
#endif
                    }
                }
            }
        }
        return T::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent* event) {
        QPoint globalPoint = event->globalPos();
        if (m_bLeftPressed) {
            bool bIgnore = true;
            QList<QScreen*> screens = QGuiApplication::screens();
            for (int i = 0; i < screens.size(); i++) {
                QScreen* pScreen = screens[i];
                QRect geometryRect = pScreen->availableGeometry();
                if (geometryRect.contains(globalPoint)) {
                    bIgnore = false;
                    break;
                }
            }

            if (bIgnore) {
                event->ignore();
                return;
            }

            if (m_Direction != Direction::NONE) {
                QRect rect = this->rect();
                QPoint tl = mapToGlobal(rect.topLeft());
                QPoint rb = mapToGlobal(rect.bottomRight());

                QRect rMove(tl, rb);

                switch (m_Direction) {
                    case Direction::LEFT:
                        if (rb.x() - globalPoint.x() <= this->minimumWidth())
                            rMove.setX(tl.x());
                        else
                            rMove.setX(globalPoint.x());
                        break;
                    case Direction::RIGHT:
                        rMove.setWidth(globalPoint.x() - tl.x());
                        break;
                    case Direction::UP:
                        if (rb.y() - globalPoint.y() <= this->minimumHeight())
                            rMove.setY(tl.y());
                        else
                            rMove.setY(globalPoint.y());
                        break;
                    case Direction::DOWN:
                        rMove.setHeight(globalPoint.y() - tl.y());
                        break;
                    case Direction::LEFTTOP:
                        if (rb.x() - globalPoint.x() <= this->minimumWidth())
                            rMove.setX(tl.x());
                        else
                            rMove.setX(globalPoint.x());
                        if (rb.y() - globalPoint.y() <= this->minimumHeight())
                            rMove.setY(tl.y());
                        else
                            rMove.setY(globalPoint.y());
                        break;
                    case Direction::RIGHTTOP:
                        rMove.setWidth(globalPoint.x() - tl.x());
                        rMove.setY(globalPoint.y());
                        break;
                    case Direction::LEFTBOTTOM:
                        rMove.setX(globalPoint.x());
                        rMove.setHeight(globalPoint.y() - tl.y());
                        break;
                    case Direction::RIGHTBOTTOM:
                        rMove.setWidth(globalPoint.x() - tl.x());
                        rMove.setHeight(globalPoint.y() - tl.y());
                        break;
                    default:
                        break;
                }
                this->setGeometry(rMove);
            }
            else {
                if (!m_bUseSystemMove) {
                    this->move(event->globalPos() - m_DragPos);
                }
                event->accept();
            }
        }
        else {
            region(globalPoint);
        }

        return T::mouseMoveEvent(event);
    }

    void region(const QPoint& cursorGlobalPoint) {
        if (!m_bResizeable)
            return;

        QRect rect = this->contentsRect();
        QPoint tl = mapToGlobal(rect.topLeft());
        QPoint rb = mapToGlobal(rect.bottomRight());
        int x = cursorGlobalPoint.x();
        int y = cursorGlobalPoint.y();

        if (tl.x() + m_iResizeRegionPadding >= x && tl.x() <= x &&
            tl.y() + m_iResizeRegionPadding >= y && tl.y() <= y) {
            m_Direction = Direction::LEFTTOP;
            this->setCursor(QCursor(Qt::SizeFDiagCursor));
        }
        else if (x >= rb.x() - m_iResizeRegionPadding && x <= rb.x() &&
                 y >= rb.y() - m_iResizeRegionPadding && y <= rb.y()) {
            m_Direction = Direction::RIGHTBOTTOM;
            this->setCursor(QCursor(Qt::SizeFDiagCursor));
        }
        else if (x <= tl.x() + m_iResizeRegionPadding && x >= tl.x() &&
                 y >= rb.y() - m_iResizeRegionPadding && y <= rb.y()) {
            m_Direction = Direction::LEFTBOTTOM;
            this->setCursor(QCursor(Qt::SizeBDiagCursor));
        }
        else if (x <= rb.x() && x >= rb.x() - m_iResizeRegionPadding && y >= tl.y() &&
                 y <= tl.y() + m_iResizeRegionPadding) {
            m_Direction = Direction::RIGHTTOP;
            this->setCursor(QCursor(Qt::SizeBDiagCursor));
        }
        else if (x <= tl.x() + m_iResizeRegionPadding && x >= tl.x()) {
            m_Direction = Direction::LEFT;
            this->setCursor(QCursor(Qt::SizeHorCursor));
        }
        else if (x <= rb.x() && x >= rb.x() - m_iResizeRegionPadding) {
            m_Direction = Direction::RIGHT;
            this->setCursor(QCursor(Qt::SizeHorCursor));
        }
        else if (y >= tl.y() && y <= tl.y() + m_iResizeRegionPadding) {
            m_Direction = Direction::UP;
            this->setCursor(QCursor(Qt::SizeVerCursor));
        }
        else if (y <= rb.y() && y >= rb.y() - m_iResizeRegionPadding) {
            m_Direction = Direction::DOWN;
            this->setCursor(QCursor(Qt::SizeVerCursor));
        }
        else {
            m_Direction = Direction::NONE;
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }

    void mouseReleaseEvent(QMouseEvent* event) {
        m_bLeftPressed = false;
        if (m_Direction != Direction::NONE) {
            m_Direction = Direction::NONE;
            this->releaseMouse();
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
        return QWidget::mouseReleaseEvent(event);
    }

    void resizeEvent(QResizeEvent* event) { return QWidget::resizeEvent(event); }

   protected:
    bool m_bLeftPressed;
    bool m_bResizeable;
    bool m_bUseSystemMove;
    Direction m_Direction;
    int m_iResizeRegionPadding;
    QPoint m_DragPos;
    QVector<QWidget*> m_titlebarWidget;
};

inline void loadStyleSheetFile(const QString& sheetName, QWidget* widget) {
    if (widget) {
        QString qss;
        QFile qssFile(sheetName);
        qssFile.open(QFile::ReadOnly);
        if (qssFile.isOpen()) {
            qss = QString::fromUtf8(qssFile.readAll());
            widget->setStyleSheet(qss);
            qssFile.close();
        }
    }
}

#endif