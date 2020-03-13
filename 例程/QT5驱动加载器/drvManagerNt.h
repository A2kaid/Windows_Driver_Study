#ifndef __HELPER_SRCDRVMANAGER_DRVMANAGER_DRV_MANAGER_NT_H__
#define __HELPER_SRCDRVMANAGER_DRVMANAGER_DRV_MANAGER_NT_H__

#include "drvManager.h"

class CDrvManagerNt : public CDrvManager
{
public:
    CDrvManagerNt();
    virtual ~CDrvManagerNt();

    /// ��Ҫʵ�ֵĴ��麯��
    virtual DWORD   InstallOpt();

private:
    /// ���ݳ�ʼ��, ����ʼ���������麯��, ÿ����������¶���
    void DataInit();    ///< ���ݳ�ʼ��
    void DataUnInit();  ///< ���ݷ���ʼ��
};

// #ifndef __HELPER_SRCDRVMANAGER_DRVMANAGER_DRV_MANAGER_NT_H__
#endif
