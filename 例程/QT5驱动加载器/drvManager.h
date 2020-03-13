#ifndef __HELPER_SRCDRVMANAGER_DRVMANAGER_H__
#define __HELPER_SRCDRVMANAGER_DRVMANAGER_H__

#define DRV_MANAGER_ERR_BASE        0x80000000
#define DRV_MANAGER_OK              S_OK

#define DRV_MANAGER_NOT_IMP                 DRV_MANAGER_ERR_BASE + 1
#define DRV_MANAGER_INVALID_PARAM           DRV_MANAGER_ERR_BASE + 2
#define DRV_MANAGER_OPEN_SCM                DRV_MANAGER_ERR_BASE + 3
#define DRV_MANAGER_CREATE_SERVICE          DRV_MANAGER_ERR_BASE + 4
#define DRV_MANAGER_START_SERVICE           DRV_MANAGER_ERR_BASE + 5
#define DRV_MANAGER_SERVICE_CONTROL_STOP    DRV_MANAGER_ERR_BASE + 6
#define DRV_MANAGER_NOT_SERVICE_STOPPED     DRV_MANAGER_ERR_BASE + 7
#define DRV_MANAGER_DELETE_SERVICE          DRV_MANAGER_ERR_BASE + 8
#define DRV_MANAGER_OPEN_SERVICE            DRV_MANAGER_ERR_BASE + 9

#ifndef SIZEOF_WSTRING
#define SIZEOF_WSTRING(x)   (_tcslen(x) * sizeof(wchar_t))
#endif

#include <string>
#include <Windows.h>

class CDrvManager
{
public:
    enum e_DrvType
    {
        e_DrvTypeNt =   0,
        e_DrvTypeSFilter,
        e_DrvTypeMiniFilter,
        e_DrvTypeUnknown
    };

public:
    CDrvManager();
    virtual ~CDrvManager();

    DWORD   Install();
    DWORD   Run();
    DWORD   Stop();
    DWORD   UnInstall();

    virtual DWORD   InstallOpt() = 0;
    virtual DWORD   RunOpt();
    virtual DWORD   StopOpt();
    virtual DWORD   UnInstallOpt();

    virtual DWORD   RegOpt_Install() {return DRV_MANAGER_OK;}
    virtual DWORD   RegOpt_UnInstall() {return DRV_MANAGER_OK;}

    void SetDriverType(const e_DrvType & eDriverType);
    CDrvManager::e_DrvType GetDriverType();

    void SetTargetName(const wchar_t * pcTargetName);
    const wchar_t * GetTargetName();

    void SetDriverPathName(const wchar_t * pcDriverPathName);
    const wchar_t * GetDriverPathName();

private:
    void DataInit();
    void DataUnInit();

private:
    e_DrvType       m_DrvType;
    std::wstring    m_strTargetName;
    std::wstring    m_strDriverPathName;
};

#endif  // #ifndef __HELPER_SRCDRVMANAGER_DRVMANAGER_H__
