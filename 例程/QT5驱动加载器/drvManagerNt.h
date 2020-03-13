#ifndef __HELPER_SRCDRVMANAGER_DRVMANAGER_DRV_MANAGER_NT_H__
#define __HELPER_SRCDRVMANAGER_DRVMANAGER_DRV_MANAGER_NT_H__

#include "drvManager.h"

class CDrvManagerNt : public CDrvManager
{
public:
    CDrvManagerNt();
    virtual ~CDrvManagerNt();

    /// 需要实现的纯虚函数
    virtual DWORD   InstallOpt();

private:
    /// 数据初始化, 反初始化不能是虚函数, 每个类必须重新定义
    void DataInit();    ///< 数据初始化
    void DataUnInit();  ///< 数据反初始化
};

// #ifndef __HELPER_SRCDRVMANAGER_DRVMANAGER_DRV_MANAGER_NT_H__
#endif
