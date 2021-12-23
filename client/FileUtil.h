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
#include <QString>
#include <string>

class FileUtil {
   public:
    static qint64 GetFileSize(const QString& filePath);
    static QString FileSizeToHuman(qint64 num);
    static bool IsSupportCompressFormat(const QString& fileName);
    static bool MoveToTrash(const QString& path);
    static QString currentUserDesktopFolder();
    static QString GetRecommandCompressPath(const QStringList& filePaths, const QString& suffix);

    static int StringIncludeTimes(const QString& src, const QString& search);

    static bool IsCompressLevel(const QString& path, int level); // level start from 0
    static bool IsInCompressFolder(const QString& folder, const QString& path);
    static QString GetNameFromCompressPath(const QString& path);
    static QString GetCompressUpLevelPath(const QString& path);
    static QStringList GetListOfDrives();
    static QString GetVolumeName(const QString& driverRoot);
    static bool RegisterFileAssociate();

};
