/// @file           \Helper\srcDrvManager\drvManager.cpp
/// @brief          ...

#include "drvManager.h"
#include <Windows.h>

CDrvManager::CDrvManager()
{
    DataInit();
}

CDrvManager::~CDrvManager()
{
    DataUnInit();
}

void CDrvManager::SetDriverType(const e_DrvType & eDriverType)
{
    m_DrvType = eDriverType;
}

void CDrvManager::SetTargetName(const wchar_t * pcTargetName)
{
    m_strTargetName = pcTargetName;
}

void CDrvManager::SetDriverPathName(const wchar_t * pcDriverPathName)
{
    m_strDriverPathName = pcDriverPathName;
}

void CDrvManager::DataInit()
{
    m_DrvType = CDrvManager::e_DrvTypeUnknown;
    m_strTargetName = L"";
    m_strDriverPathName = L"";
}

void CDrvManager::DataUnInit()
{
}

CDrvManager::e_DrvType CDrvManager::GetDriverType()
{
    return m_DrvType;
}

const wchar_t * CDrvManager::GetTargetName()
{
    return m_strTargetName.c_str();
}

const wchar_t * CDrvManager::GetDriverPathName()
{
    if (0 == m_strDriverPathName.length())
    {
        m_strDriverPathName = L".\\";
        m_strDriverPathName += m_strTargetName.c_str();
        m_strDriverPathName += L".sys";
    }

    return m_strDriverPathName.c_str();
}

DWORD   CDrvManager::Install()
{
    DWORD   dwRc    =   DRV_MANAGER_OK;

    dwRc = InstallOpt();
    if (DRV_MANAGER_OK != dwRc)
        return dwRc;

    return RegOpt_Install();
}

DWORD   CDrvManager::Run()
{
    return RunOpt();
}

DWORD   CDrvManager::Stop()
{
    return StopOpt();
}

DWORD   CDrvManager::UnInstall()
{
    DWORD   dwRc    =   DRV_MANAGER_OK;

    dwRc = UnInstallOpt();
    if (DRV_MANAGER_OK != dwRc)
        return dwRc;

    return RegOpt_UnInstall();
}

DWORD   CDrvManager::RunOpt()
{
    DWORD       dwRc        =   DRV_MANAGER_OK;
    SC_HANDLE   schManager  =   NULL;
    SC_HANDLE   schService  =   NULL;

    __try
    {
        if (NULL == GetTargetName())
        {
            dwRc = DRV_MANAGER_INVALID_PARAM;
            __leave;
        }

        schManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
        if (NULL == schManager)
        {
            dwRc = DRV_MANAGER_OPEN_SCM;
            __leave;
        }


        schService = OpenService(   schManager,
            GetTargetName(),
            SERVICE_ALL_ACCESS);
        if (NULL == schService)
        {
            dwRc = DRV_MANAGER_OPEN_SERVICE;
            __leave;
        }

        if (!StartService(schService, 0, NULL))
        {
            if( GetLastError() == ERROR_SERVICE_ALREADY_RUNNING ) 
                __leave;

            dwRc = DRV_MANAGER_START_SERVICE;
        }
    }

    __finally
    {
        if (NULL != schService)
            CloseServiceHandle(schService);

        if (NULL != schManager)
            CloseServiceHandle(schManager);
    }

    return dwRc;
}

DWORD   CDrvManager::StopOpt()
{
    DWORD           dwRc        =   DRV_MANAGER_OK;
    SC_HANDLE       schManager  =   NULL;
    SC_HANDLE       schService  =   NULL;
    SERVICE_STATUS  svcStatus;

    __try
    {
        schManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (NULL == schManager)
        {
            dwRc = DRV_MANAGER_OPEN_SCM;
            __leave;
        }

        schService = OpenService(   schManager,
            GetTargetName(),
            SERVICE_ALL_ACCESS);
        if (NULL == schService)
        {
            dwRc = DRV_MANAGER_OPEN_SERVICE;
            __leave;
        }

        if (!ControlService(schService,SERVICE_CONTROL_STOP,&svcStatus))
        {
            dwRc = DRV_MANAGER_SERVICE_CONTROL_STOP;
            __leave;
        }

        if (SERVICE_STOPPED != svcStatus.dwCurrentState)
        {
            dwRc = DRV_MANAGER_NOT_SERVICE_STOPPED;
            __leave;
        }
    }

    __finally
    {
        if (NULL != schService)
            CloseServiceHandle(schService);

        if (NULL != schManager)
            CloseServiceHandle(schManager);
    }

    return dwRc;
}

DWORD   CDrvManager::UnInstallOpt()
{
    DWORD           dwRc        =   DRV_MANAGER_OK;
    SC_HANDLE       schManager  =   NULL;
    SC_HANDLE       schService  =   NULL;

    __try
    {
        schManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if(NULL==schManager)
        {
            dwRc = DRV_MANAGER_OPEN_SCM;
            __leave;
        }

        schService = OpenService(   schManager,
            GetTargetName(),
            SERVICE_ALL_ACCESS);
        if (NULL == schService)
        {
            dwRc = DRV_MANAGER_OPEN_SERVICE;
            __leave;
        }

        if (!DeleteService(schService))
        {
            dwRc = DRV_MANAGER_DELETE_SERVICE;
            __leave;
            return FALSE;
        }
    }

    __finally
    {
        if (NULL != schService)
            CloseServiceHandle(schService);

        if (NULL != schManager)
            CloseServiceHandle(schManager);
    }

    return dwRc;
}
