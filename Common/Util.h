/*
***************************************************************************************************
* 许可声明：
* 本软件（含源码、二进制）为开源软件，遵循MIT开源许可协议（若以下声明与MIT许可协议发送冲突，以本声明为准）。
* 任何个人或组织都可以不受限制的使用、修改该软件。
* 任何个人或组织都可以以源代码和二进制的形式重新分发、使用该软件。
* 任何个人或组织都可以不受限制地将该软件（或修改后的任意版本）用于商业、非商业的用途。
* 
* 免责声明：
* 作者不对使用、修改、分发（以及其他任何形式的使用）该软件（及其他修改后的任意版本）所产生的后果负有任何法律责任。
* 作者也不对该软件的安全性、稳定性做出任何保证。
* 
* 使用、修改、分发该软件时需要保留上述声明，且默认已经同意上述声明。
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
        // TODO: 根据API协议，自行解析，以下仅为示例
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
        // TODO: 根据API协议自行解析
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
        // TODO: 根据API协议自行解析
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
        // TODO: 根据API协议自行解析
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
