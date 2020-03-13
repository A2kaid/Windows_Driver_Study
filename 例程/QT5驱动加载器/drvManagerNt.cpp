/// @file           \Helper\srcDrvManager\drvManagerNt.cpp
/// @brief          ...

#include "drvManagerNt.h"

CDrvManagerNt::CDrvManagerNt()
{
    DataInit();
}

CDrvManagerNt::~CDrvManagerNt()
{
    DataUnInit();
}

DWORD   CDrvManagerNt::InstallOpt()
{
    wchar_t         szDriverImagePath[MAX_PATH] = {0};    

    DWORD       dwRc        =   DRV_MANAGER_OK;
    SC_HANDLE   hServiceMgr =   NULL;
    SC_HANDLE   hService    =   NULL;

    __try
    {
        if ((NULL == GetTargetName()) || (NULL == GetDriverPathName()))
        {
            dwRc = DRV_MANAGER_INVALID_PARAM;
            __leave;
        }
        GetFullPathName(GetDriverPathName(), MAX_PATH, szDriverImagePath, NULL);
        hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
        if (NULL == hServiceMgr) 
        {
            dwRc = DRV_MANAGER_OPEN_SCM;
            __leave;
        }

        /// 创建驱动所对应的服务
        hService = CreateService( hServiceMgr,
            GetTargetName(),
            GetTargetName(),
            SERVICE_ALL_ACCESS,
            SERVICE_KERNEL_DRIVER,
            SERVICE_DEMAND_START,
            SERVICE_ERROR_IGNORE,
            szDriverImagePath,
            NULL,
            NULL, 
            NULL,
            NULL, 
            NULL);

        if (NULL == hService) 
        {        
            if (ERROR_SERVICE_EXISTS != GetLastError())
                dwRc =  DRV_MANAGER_CREATE_SERVICE;
        }
    }

    __finally
    {
        if (NULL != hService)
            CloseServiceHandle(hService);

        if (NULL != hServiceMgr)
            CloseServiceHandle(hServiceMgr);
    }

    return dwRc;
}


void CDrvManagerNt::DataInit()
{
    SetDriverType(CDrvManager::e_DrvTypeNt);
}

void CDrvManagerNt::DataUnInit()
{
}
