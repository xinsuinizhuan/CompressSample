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
#include "akali.h"
#include <QtWidgets>
#include <QString>
#include <QSharedPointer>
#include <assert.h>
#include <strsafe.h>
#include <functional>
#include <tchar.h>
#include <Shlwapi.h>
#include "Logger.h"
#include "FileUtil.h"
#include "ProductDef.h"

using namespace akali;

// Support Chinese
#pragma execution_character_set("utf-8")

#pragma comment(lib, "Shlwapi.lib")

QString appDataRootFolder();


#define REFRESH_STYLE(x)        \
    do {                        \
        style()->unpolish((x)); \
        style()->polish((x));   \
    } while (false)
#define REFRESH_WIDGET_STYLE(w, x)   \
    do {                             \
        (w)->style()->unpolish((x)); \
        (w)->style()->polish((x));   \
    } while (false)
