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
#pragma once
#include <string>
#include <vector>
#include "json/json.h"
#include "akali/stringencode.h"
#include "ProductDef.h"

enum FollowAction {
    FA_UNKNOWN = 0,
    DO_UPDATE = 1,
    RUN_TASK = 2,
    EXIT_APP = 3,
};

enum TaskType {
    TT_UNKNOWN = 0,
    INSTALL_SOFT = 1,
    SET_DESKTOP_LINK = 2,
    FLICK_TRAY_ICON = 3,
    FLICK_TASKBAR = 4,
    MINI_AD_PAGE = 5,
    POPUP_WIDNOW = 6
};

struct ProfitHistory {
    int id;
    std::wstring stamp;

    ProfitHistory() {
        id = 0;
    }

    void fromJsonObj(const Json::Value& root) {
        // TODO: ����APIЭ�飬���н��������½�Ϊʾ��
        //
    }
};

struct NoRecentShowHistory {
    int id;
    std::wstring stamp;

    NoRecentShowHistory() {
        id = 0;
    }

    void fromJsonObj(const Json::Value& root) {
        // TODO: ����APIЭ�����н���
        //
    }
};

struct SoftInstallHistory {
    int id;
    std::wstring stamp;

    SoftInstallHistory() {
        id = 0;
    }

    void fromJsonObj(const Json::Value& root) {
        // TODO: ����APIЭ�����н���
        //
    }
};


struct EnvCheckResult {
    bool result;
    std::vector<int> notExistRegIds;
    std::vector<int> existProcessIds;
    std::vector<NoRecentShowHistory> noRecents;
    std::vector<ProfitHistory> moneyHistorys;
    std::vector<SoftInstallHistory> softHistorys;
    FollowAction followAction;

    EnvCheckResult() {
        result = false;
        followAction = FollowAction::FA_UNKNOWN;
    }
};

struct TaskMeta {
    int id;
    std::wstring displayName;
    std::wstring fileName;
    std::wstring url;
    std::wstring md5;
    std::wstring cmd;
    std::wstring resUrl;
    std::wstring resSavePath;
    bool delLink;
    std::wstring linkName;
    int regRoot;
    std::wstring regPath;
    bool check;
    bool forceInstall;
    int insrp;
    bool launch;

    TaskMeta() {
        id = 0;
        delLink = false;
        regRoot = 0;
        check = false;
        forceInstall = false;
        insrp = 0;
        launch = false;
    }

    void fromJsonObj(const Json::Value& root) {
        // TODO: ����APIЭ�����н���
        //
    }
};

struct FullTask {
    std::vector<TaskMeta> tms;
    TaskType taskType;

    FullTask() {
        taskType = TaskType::TT_UNKNOWN;
    }
};

class Util {
   public:
    static std::wstring GetChannel();
    static int GetAppVersion();
    static void SetAppVersion(int appver);
    static int GetOSMajorVersion();
    static int GetOSMinorVersion();

    static std::wstring GetNoRecentHistoryJSON();
    static std::wstring GetSoftHistoryJSON();
    static std::wstring GetMoneyHistoryJSON();

    static std::vector<NoRecentShowHistory> ParseNoRecentHistory(const std::wstring& strJSON);
    static std::vector<SoftInstallHistory> ParseSoftHistory(const std::wstring& strJSON);
    static std::vector<ProfitHistory> ParseMoneyHistory(const std::wstring& strJSON);

    static void SetNoRecentHistory(const std::vector<NoRecentShowHistory>& norecent);
    static void SetSoftHistory(const std::vector<SoftInstallHistory>& his);
    static void SetProfitHistory(const std::vector<ProfitHistory>& moneyHis);

    static bool CreateProcessWarpper(const std::wstring& path, const std::wstring& cmd);
};
