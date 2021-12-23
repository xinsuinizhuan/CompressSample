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
#include "HttpManager.h"
#include <windows.h>
#include <TlHelp32.h>
#include "json/json.h"
#include "akali/trace.h"
#include "akali/registry.h"
#include "akali/stringencode.h"

using namespace akali;

HttpManager::HttpManager() {
}

HttpManager::~HttpManager() {
    for (int i = 0; i < httpClients_.size(); i++) {
        if (httpClients_[i]) {
            httpClients_[i]->Abort();
            httpClients_[i]->Wait(200);
        }
    }

    httpClients_.clear();
}

bool HttpManager::IsRegExist(int root, const std::wstring& path) {
    if (path.length() == 0)
        return false;

    HKEY hRoot = NULL;
    if (root == 1)
        hRoot = HKEY_LOCAL_MACHINE;
    else if (root == 2)
        hRoot = HKEY_CURRENT_USER;
    else
        return false;

    HKEY hKey = NULL;
    LSTATUS nRes;
    nRes = RegOpenKeyExW(hRoot, path.c_str(), 0, KEY_READ, &hKey);
    if (nRes == ERROR_SUCCESS && hKey) {
        return true;
    }
    return false;
}

bool HttpManager::IsProcessExist(const std::wstring& name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!snapshot) {
        return false;
    }
    bool bExist = false;
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);
    if (Process32First(snapshot, &process)) {
        do {
            if (lstrcmpi(process.szExeFile, name.c_str()) == 0) {
                bExist = true;
                break;
            }
        } while (Process32Next(snapshot, &process));
    }
    CloseHandle(snapshot);

    return bExist;
}

void HttpManager::asyncGetEnvCheckInfo(int osMajor,
                                       int osMinor,
                                       int appVer,
                                       const std::wstring& appid,
                                       const std::wstring& uid,
                                       std::function<void(const EnvCheckResult&)> cb) {
    std::shared_ptr<Http::Client> hc = std::make_shared<Http::Client>();
    httpClients_.push_back(hc);

    std::shared_ptr<Http::RequestDatagram> reqDG = std::make_shared<Http::RequestDatagram>();

    Json::Value root;
    // TODO: ����APIЭ���������������
    //

  

    std::string strBody = root.toStyledString();

    reqDG->SetUrl(GET_ENV_CHECK_INFO_API);
    reqDG->SetMethod(Http::RequestDatagram::METHOD::POST);
    reqDG->SetBody(strBody.c_str(), strBody.size());

    hc->Request(
        reqDG, [cb, this](int ret, unsigned long ms, const Http::ResponseDatagram& rsp) {
            EnvCheckResult envChkRet;

            if (ret != 0) {
                if (cb)
                    cb(envChkRet);
                return;
            }

            try {
                Json::CharReaderBuilder readerBuilder;
                Json::CharReader* reader = readerBuilder.newCharReader();
                Json::Value root;
                Json::String err;
                if (!reader->parse((const char*)(rsp.GetBody()),
                                   ((const char*)rsp.GetBody() + rsp.GetBodySize()), &root, &err)) {
                    if (cb)
                        cb(envChkRet);
                    return;
                }

                // TODO: ����APIЭ�����н���
                //


                if (cb)
                    cb(envChkRet);
            } catch (std::exception& e) {
                akali::TraceMsgA("%s\n", e.what() ? e.what() : "");
            }
        },
        true);
}

void HttpManager::asyncGetSoftList(int osMajor,
                                   int osMinor,
                                   int appver,
                                   const std::wstring& appid,
                                   const std::wstring& uid,
                                   const EnvCheckResult& envChkRet,
                                   std::function<void(FullTask)> cb) {
    // TODO
    //
}

void HttpManager::asyncGetTaskList(int osMajor,
                                   int osMinor,
                                   int appver,
                                   const std::wstring& appid,
                                   const std::wstring& uid,
                                   int type,
                                   const EnvCheckResult& envChkRet,
                                   std::function<void(FullTask)> cb) {
    std::shared_ptr<Http::Client> hc = std::make_shared<Http::Client>();
    httpClients_.push_back(hc);

    std::shared_ptr<Http::RequestDatagram> reqDG = std::make_shared<Http::RequestDatagram>();

    Json::Value root;
    // TODO: ����APIЭ�������������Body
    //


    std::string strBody = root.toStyledString();

    reqDG->SetUrl(GET_TASK_LIST_API);
    reqDG->SetMethod(Http::RequestDatagram::METHOD::POST);
    reqDG->SetBody(strBody.c_str(), strBody.size());

    hc->Request(
        reqDG, [cb, this](int ret, unsigned long ms, const Http::ResponseDatagram& rsp) {
            FullTask fullTask;

            try {
                Json::CharReaderBuilder readerBuilder;
                Json::CharReader* reader = readerBuilder.newCharReader();
                Json::Value root;
                Json::String err;
                if (!reader->parse((const char*)(rsp.GetBody()),
                                   ((const char*)rsp.GetBody() + rsp.GetBodySize()), &root, &err)) {
                    if (cb)
                        cb(fullTask);
                    return;
                }

                // TODO: ����APIЭ�����н���
                //
                

                if (cb)
                    cb(fullTask);
            } catch (std::exception& e) {
                akali::TraceMsgA("%s\n", e.what() ? e.what() : "");
            }
        },
        true);
}
