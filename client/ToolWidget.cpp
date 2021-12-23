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
#include "stdafx.h"
#include "ToolWidget.h"
#include "Util.h"

const int kColCount = 2;

ToolItem::ToolItem(const TaskMeta& tm, QWidget* parent /*= Q_NULLPTR*/) :
    QWidget(parent), toolMeta_(tm) {
    setupUi();

    connect(pushButton_, &QPushButton::clicked, this, [this]() {
        emit itemClicked(toolMeta_);
    });
}

ToolItem::~ToolItem() {
}

void ToolItem::setupUi() {
    setObjectName("toolItem");
    pushButton_ = new QPushButton();
    pushButton_->setObjectName("toolItem_pushButton");
    pushButton_->setFixedSize(42, 42);
    pushButton_->setCursor(QCursor(Qt::PointingHandCursor));
    pushButton_->setStyleSheet(QString("#toolItem_pushButton{border:none; image:url(%1);}").arg(QString::fromStdWString(toolMeta_.resSavePath)));

    labelName_ = new QLabel();
    labelName_->setObjectName("toolItem_labelName");
    labelName_->setFixedHeight(16);
    labelName_->setText(QString::fromStdWString(toolMeta_.displayName));

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setContentsMargins(10, 10, 10, 10);
    vl->setSpacing(0);
    vl->addWidget(pushButton_);
    vl->addSpacing(4);
    vl->addWidget(labelName_);

    this->setLayout(vl);

    this->setFixedSize(62, 82);
}

ToolWidget::ToolWidget(QWidget* parent /*= Q_NULLPTR*/) :
    FramelessWindow<QWidget>(true, parent) {
    setupUi();

    querySoftList();
}

ToolWidget::~ToolWidget() {
    for (int i = 0; i < teemos_.size(); i++) {
        if (teemos_[i]) {
            teemos_[i]->stop();
        }
    }

    teemos_.clear();
}

void ToolWidget::setupUi() {
    setObjectName("toolWidget");

    toolTable_ = new QTableWidget();
    toolTable_->setObjectName("toolWidget_toolTable");
    toolTable_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolTable_->setShowGrid(false);
    toolTable_->setFocusPolicy(Qt::NoFocus);
    toolTable_->setSelectionMode(QAbstractItemView::NoSelection);
    toolTable_->horizontalHeader()->hide();
    toolTable_->verticalHeader()->hide();
    toolTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    toolTable_->setColumnCount(kColCount);
    toolTable_->horizontalScrollBar()->hide();
    toolTable_->verticalScrollBar()->hide();

    QHBoxLayout* hl = new QHBoxLayout();
    hl->setContentsMargins(5, 5, 5, 5);
    hl->setSpacing(0);
    hl->addWidget(toolTable_);

    centralWidget_ = new QWidget();
    centralWidget_->setObjectName("toolWidget_centralWidget");
    centralWidget_->setLayout(hl);

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(0);
    vl->addWidget(centralWidget_);

    this->setLayout(vl);
    this->setFixedSize(134, 174);
}

void ToolWidget::leaveEvent(QEvent* event) {
    this->hide();
    QWidget::leaveEvent(event);
}

void ToolWidget::focusOutEvent(QFocusEvent* event) {
    this->hide();
    QWidget::focusOutEvent(event);
}

void ToolWidget::updateList() {
    toolTable_->clear();

    toolTable_->setRowCount(toolMetas_.size() / kColCount + 1);
    for (int i = 0; i < toolMetas_.size(); i++) {
        int row = i / kColCount;
        int col = i % kColCount;

        ToolItem* pItem = new ToolItem(toolMetas_[i]);
        connect(pItem, &ToolItem::itemClicked, this, &ToolWidget::onItemClicked);
        toolTable_->setCellWidget(row, col, pItem);
        toolTable_->setColumnWidth(col, 62);
        toolTable_->setRowHeight(row, 82);
    }
}

void ToolWidget::querySoftList() {
    std::wstring uid = L"";
    HttpManager::Instance()->asyncGetEnvCheckInfo(
        Util::GetOSMajorVersion(),
        Util::GetOSMinorVersion(),
        Util::GetAppVersion(),
        Util::GetChannel(),
        uid,
        [uid, this](const EnvCheckResult& envChkRet) {
            if (!envChkRet.result)
                return;

            HttpManager::Instance()->asyncGetSoftList(
                Util::GetOSMajorVersion(),
                Util::GetOSMinorVersion(),
                Util::GetAppVersion(),
                Util::GetChannel(),
                uid,
                envChkRet,
                [this](FullTask fullTask) {
                    std::vector<TaskMeta> displayMetas;
                    for (int i = 0; i < fullTask.tms.size(); i++) {
                        std::wstring resUrlMd5 = akali::Utf8ToUnicode(
                            akali::GetStringMd5(fullTask.tms[i].resUrl.c_str(), fullTask.tms[i].resUrl.length() * sizeof(wchar_t)));
                        std::wstring resSavePath = appDataRootFolder().toStdWString() + resUrlMd5 + L".png";

                        if (_waccess_s(resSavePath.c_str(), 0) == 0) {
                            fullTask.tms[i].resSavePath = resSavePath;
                        }
                        else {
                            std::shared_ptr<teemo::Teemo> tmo = std::make_shared<teemo::Teemo>();
                            teemos_.push_back(tmo);

                            tmo->setThreadNum(2);
                            tmo->setRedirectedUrlCheckEnabled(true);
                            teemo::Result ret = tmo->start(akali::UnicodeToUtf8(fullTask.tms[i].resUrl), akali::UnicodeToUtf8(resSavePath), nullptr, nullptr, nullptr).get();
                            if (ret == teemo::Result::SUCCESSED) {
                                fullTask.tms[i].resSavePath = resSavePath;
                            }
                            else if (ret == teemo::Result::CANCELED) {
                                return;
                            }
                            else {
                                continue;
                            }
                        }

                        displayMetas.push_back(fullTask.tms[i]);

                        if (displayMetas.size() == 4)
                            break;
                    }

                    toolMetas_ = displayMetas;
                    auto metaObject = this->metaObject();
                    QMetaMethod method = metaObject->method(metaObject->indexOfMethod(QMetaObject::normalizedSignature("updateList(std::vector<SoftMeta>)")));
                    method.invoke(this, Qt::QueuedConnection, Q_ARG(std::vector<TaskMeta>, displayMetas));
                });
        });
}

void ToolWidget::onItemClicked(TaskMeta tm) {
}
