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
#include "HttpClient.h"
#include <vector>
#include <string>
#include "Util.h"
#include "akali/singleton.hpp"

// TODO: �ڴ�����API��ַ
//
#define GET_ENV_CHECK_INFO_API ""
#define GET_TASK_LIST_API ""

class HttpManager : public akali::Singleton<HttpManager> {
   public:
    ~HttpManager();

    static bool IsRegExist(int root, const std::wstring& path);
    static bool IsProcessExist(const std::wstring& name);

    void asyncGetEnvCheckInfo(int osMajor,
                              int osMinor,
                              int appVer,
                              const std::wstring& appid,
                              const std::wstring& uid,
                              std::function<void(const EnvCheckResult&)> cb);

    void asyncGetSoftList(int osMajor,
                          int osMinor,
                          int appver,
                          const std::wstring& appid,
                          const std::wstring& uid,
                          const EnvCheckResult& envChkRet,
                          std::function<void(FullTask)> cb);

    void asyncGetTaskList(int osMajor,
                          int osMinor,
                          int appver,
                          const std::wstring& appid,
                          const std::wstring& uid,
                          int type,
                          const EnvCheckResult& envChkRet,
                          std::function<void(FullTask)> cb);

   private:
    HttpManager();
    std::vector<std::shared_ptr<Http::Client>> httpClients_;

    SINGLETON_CLASS_DECLARE(HttpManager);
};
