/*
 * BootMaster/main.c
 * Main code for the boot menu
 *
 * Copyright (c) 2006-2010 Christoph Pfisterer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Christoph Pfisterer nor the names of the
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Modifications copyright (c) 2012-2021 Roderick W. Smith
 *
 * Modifications distributed under the terms of the GNU General Public
 * License (GPL) version 3 (GPLv3), or (at your option) any later version.
 */
/*
 * Modified for RefindPlus
 * Copyright (c) 2020-2021 Dayo Akanji (sf.net/u/dakanji/profile)
 * Portions Copyright (c) 2021 Joe van Tunen (joevt@shaw.ca)
 *
 * Modifications distributed under the preceding terms.
 */

#include "global.h"
#include "config.h"
#include "screenmgt.h"
#include "launch_legacy.h"
#include "lib.h"
#include "icns.h"
#include "install.h"
#include "menu.h"
#include "mok.h"
#include "apple.h"
#include "mystrings.h"
#include "security_policy.h"
#include "driver_support.h"
#include "launch_efi.h"
#include "scan.h"
#include "../include/refit_call_wrapper.h"
#include "../libeg/efiConsoleControl.h"
#include "../libeg/efiUgaDraw.h"
#include "../include/version.h"
#include "../libeg/libeg.h"

#ifndef __MAKEWITH_GNUEFI
#define LibLocateProtocol EfiLibLocateProtocol
#endif

INT16 NowYear   = 0;
INT16 NowMonth  = 0;
INT16 NowDay    = 0;
INT16 NowHour   = 0;
INT16 NowMinute = 0;
INT16 NowSecond = 0;

REFIT_MENU_SCREEN *MainMenu = NULL;

REFIT_CONFIG GlobalConfig = {
    /* CustomScreenBG = */ FALSE,
    /* TextOnly = */ FALSE,
    /* ScanAllLinux = */ TRUE,
    /* DeepLegacyScan = */ FALSE,
    /* EnableAndLockVMX = */ FALSE,
    /* FoldLinuxKernels = */ TRUE,
    /* EnableMouse = */ FALSE,
    /* EnableTouch = */ FALSE,
    /* HiddenTags = */ TRUE,
    /* UseNvram = */ FALSE,
    /* IgnorePreviousBoot = */ FALSE,
    /* IgnoreHiddenIcons = */ FALSE,
    /* ExternalHiddenIcons = */ FALSE,
    /* PreferHiddenIcons = */ FALSE,
    /* TextRenderer = */ FALSE,
    /* UgaPassThrough = */ FALSE,
    /* ProvideConsoleGOP = */ FALSE,
    /* ReloadGOP = */ TRUE,
    /* UseDirectGop = */ FALSE,
    /* ContinueOnWarning = */ FALSE,
    /* ForceTRIM = */ FALSE,
    /* DisableCompatCheck = */ FALSE,
    /* DisableAMFI = */ FALSE,
    /* SupplyNVME = */ TRUE,
    /* SupplyAPFS = */ TRUE,
    /* SilenceAPFS = */ TRUE,
    /* SyncAPFS = */ TRUE,
    /* ProtectNVRAM = */ TRUE,
    /* ScanAllESP = */ FALSE,
    /* TagsHelp = */ TRUE,
    /* NormaliseCSR = */ FALSE,
    /* ShutdownAfterTimeout = */ FALSE,
    /* Install = */ FALSE,
    /* WriteSystemdVars = */ FALSE,
    /* RequestedScreenWidth = */ 0,
    /* RequestedScreenHeight = */ 0,
    /* BannerBottomEdge = */ 0,
    /* RequestedTextMode = */ DONT_CHANGE_TEXT_MODE,
    /* HideUIFlags = */ 0,
    /* MaxTags = */ 0,
    /* GraphicsFor = */ GRAPHICS_FOR_OSX,
    /* LegacyType = */ LEGACY_TYPE_MAC,
    /* ScanDelay = */ 0,
    /* MouseSpeed = */ 4,
    /* IconSizes = */ {
        DEFAULT_BIG_ICON_SIZE / 4,
        DEFAULT_SMALL_ICON_SIZE,
        DEFAULT_BIG_ICON_SIZE,
        DEFAULT_MOUSE_SIZE
    },
    /* BannerScale = */ BANNER_NOSCALE,
    /* ScreensaverTime = */ 0,
    /* Timeout = */ 0,
    /* ScaleUI = */ 0,
    /* ActiveCSR = */ 0,
    /* LogLevel = */ 0,
    /* ScreenR = */ -1,
    /* ScreenG = */ -1,
    /* ScreenB = */ -1,
    /* *DiscoveredRoot = */ NULL,
    /* *SelfDevicePath = */ NULL,
    /* *BannerFileName = */ NULL,
    /* *ScreenBackground = */ NULL,
    /* *ConfigFilename = */ CONFIG_FILE_NAME,
    /* *SelectionSmallFileName = */ NULL,
    /* *SelectionBigFileName = */ NULL,
    /* *DefaultSelection = */ NULL,
    /* *AlsoScan = */ NULL,
    /* *DontScanVolumes = */ NULL,
    /* *DontScanDirs = */ NULL,
    /* *DontScanFiles = */ NULL,
    /* *DontScanTools = */ NULL,
    /* *DontScanFirmware = */ NULL,
    /* *WindowsRecoveryFiles = */ NULL,
    /* *MacOSRecoveryFiles = */ NULL,
    /* *DriverDirs = */ NULL,
    /* *IconsDir = */ NULL,
    /* *SetBootArgs = */ NULL,
    /* *ExtraKernelVersionStrings = */ NULL,
    /* *SpoofOSXVersion = */ NULL,
    /* CsrValues = */ NULL,
    /* ShowTools = */ {
        TAG_SHELL,
        TAG_MEMTEST,
        TAG_GDISK,
        TAG_RECOVERY_APPLE,
        TAG_RECOVERY_WINDOWS,
        TAG_MOK_TOOL,
        TAG_ABOUT,
        TAG_HIDDEN,
        TAG_SHUTDOWN,
        TAG_REBOOT,
        TAG_FIRMWARE,
        TAG_FWUPDATE_TOOL,
        0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

#define BOOTKICKER_FILES L"\\EFI\\BOOT\\tools_x64\\x64_BootKicker.efi,\\EFI\\BOOT\\tools_x64\\BootKicker_x64.efi,\
\\EFI\\BOOT\\tools_x64\\BootKicker.efi,\\EFI\\BOOT\\tools\\x64_BootKicker.efi,\\EFI\\BOOT\\tools\\BootKicker_x64.efi,\
\\EFI\\BOOT\\tools\\BootKicker.efi,\\EFI\\tools\\x64_BootKicker.efi,\\EFI\\tools\\BootKicker_x64.efi,\
\\EFI\\tools\\BootKicker.efi,\\EFI\\tools_x64\\x64_BootKicker.efi,\\EFI\\tools_x64\\BootKicker_x64.efi,\
\\EFI\\tools_x64\\BootKicker.efi,\\EFI\\x64_BootKicker.efi,\\EFI\\BootKicker_x64.efi,\\EFI\\BootKicker.efi,\
\\x64_BootKicker.efi,\\BootKicker_x64.efi,\\BootKicker.efi"

#define NVRAMCLEAN_FILES L"\\EFI\\BOOT\\tools_x64\\x64_CleanNvram.efi,\\EFI\\BOOT\\tools_x64\\CleanNvram_x64.efi,\
\\EFI\\BOOT\\tools_x64\\CleanNvram.efi,\\EFI\\BOOT\\tools\\x64_CleanNvram.efi,\\EFI\\BOOT\\tools\\CleanNvram_x64.efi,\
\\EFI\\BOOT\\tools\\CleanNvram.efi,\\EFI\\tools\\x64_CleanNvram.efi,\\EFI\\tools\\CleanNvram_x64.efi,\
\\EFI\\tools\\CleanNvram.efi,\\EFI\\tools_x64\\x64_CleanNvram.efi,\\EFI\\tools_x64\\CleanNvram_x64.efi,\
\\EFI\\tools_x64\\CleanNvram.efi,\\EFI\\x64_CleanNvram.efi,\\EFI\\CleanNvram_x64.efi,\\EFI\\CleanNvram.efi,\
\\x64_CleanNvram.efi,\\CleanNvram_x64.efi,\\CleanNvram.efi"

CHAR16                *VendorInfo           = NULL;
CHAR16                *gHiddenTools         = NULL;
BOOLEAN                IsBoot               = FALSE;
BOOLEAN                SetSysTab            = FALSE;
BOOLEAN                ConfigWarn           = FALSE;
BOOLEAN                NativeLogger         = FALSE;
BOOLEAN                ranCleanNvram        = FALSE;
BOOLEAN                FlushFailedTag       = FALSE;
BOOLEAN                FlushFailReset       = FALSE;
BOOLEAN                WarnVersionEFI       = FALSE;
BOOLEAN                WarnRevisionUEFI     = FALSE;
BOOLEAN                WarnMissingQVInfo    = FALSE;
BOOLEAN                SecureBootFailure    = FALSE;
EFI_GUID               RefindPlusGuid       = REFINDPLUS_GUID;
EFI_SET_VARIABLE       AltSetVariable;
EFI_OPEN_PROTOCOL      OrigOpenProtocol;
EFI_HANDLE_PROTOCOL    OrigHandleProtocol;

#if REFIT_DEBUG > 0
UINTN   AppleFramebuffers = 0;

extern CHAR16 *OffsetNext;
#endif

extern VOID   InitBooterLog (VOID);

extern EFI_STATUS        RP_ApfsConnectDevices (VOID);
extern EFI_STATUS EFIAPI NvmExpressLoad (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
);


// Link to Cert GUIDs in mok/guid.c
extern EFI_GUID X509_GUID;
extern EFI_GUID RSA2048_GUID;
extern EFI_GUID PKCS7_GUID;
extern EFI_GUID EFI_CERT_SHA256_GUID;

extern EFI_FILE *gVarsDir;

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *GOPDraw;

//
// misc functions
//


static
VOID InitMainMenu (VOID) {
    REFIT_MENU_SCREEN MainMenuSrc = {
        L"Main Menu",
        NULL, 0,
        NULL, 0,
        NULL, 0,
        L"Automatic Boot",
        L"Use Arrow Keys to Move Cursor; 'Enter' to Boot;",
        L"'Insert', 'Tab', or 'F2' for More Options; 'Esc' or 'Backspace' to Refresh"
    };
    FreeMenuScreen (&MainMenu);
    MainMenu = CopyMenuScreen (&MainMenuSrc);
} // static VOID InitMainMenu()

static
EFI_STATUS EFIAPI gRTSetVariableEx (
    IN  CHAR16    *VariableName,
    IN  EFI_GUID  *VendorGuid,
    IN  UINT32     Attributes,
    IN  UINTN      VariableSize,
    IN  VOID      *VariableData
) {
    EFI_STATUS     Status;
    EFI_GUID       WinGuid               = MICROSOFT_VENDOR_GUID;
    EFI_GUID       X509Guid              = X509_GUID;
    EFI_GUID       PKCS7Guid             = PKCS7_GUID;
    EFI_GUID       Sha001Guid            = EFI_CERT_SHA1_GUID;
    EFI_GUID       Sha224Guid            = EFI_CERT_SHA224_GUID;
    EFI_GUID       Sha256Guid            = EFI_CERT_SHA256_GUID;
    EFI_GUID       Sha384Guid            = EFI_CERT_SHA384_GUID;
    EFI_GUID       Sha512Guid            = EFI_CERT_SHA512_GUID;
    EFI_GUID       RSA2048Guid           = RSA2048_GUID;
    EFI_GUID       RSA2048Sha1Guid       = EFI_CERT_RSA2048_SHA1_GUID;
    EFI_GUID       RSA2048Sha256Guid     = EFI_CERT_RSA2048_SHA256_GUID;
    EFI_GUID       TypeRSA2048Sha256Guid = EFI_CERT_TYPE_RSA2048_SHA256_GUID;

    #if REFIT_DEBUG > 0
    EFI_STATUS  LogStatus;
    #endif

    BOOLEAN BlockCert = (
        MyStrStr (VendorInfo, L"Apple") &&
        (
            GuidsAreEqual (VendorGuid, &WinGuid) ||
            GuidsAreEqual (VendorGuid, &X509Guid) ||
            GuidsAreEqual (VendorGuid, &PKCS7Guid) ||
            GuidsAreEqual (VendorGuid, &Sha001Guid) ||
            GuidsAreEqual (VendorGuid, &Sha224Guid) ||
            GuidsAreEqual (VendorGuid, &Sha256Guid) ||
            GuidsAreEqual (VendorGuid, &Sha384Guid) ||
            GuidsAreEqual (VendorGuid, &Sha512Guid) ||
            GuidsAreEqual (VendorGuid, &RSA2048Guid) ||
            GuidsAreEqual (VendorGuid, &RSA2048Sha1Guid) ||
            GuidsAreEqual (VendorGuid, &RSA2048Sha256Guid) ||
            GuidsAreEqual (VendorGuid, &TypeRSA2048Sha256Guid)
        )
    );

    if (!BlockCert) {
        Status = AltSetVariable (
            VariableName,
            VendorGuid,
            Attributes,
            VariableSize,
            VariableData
        );

        #if REFIT_DEBUG > 0
        LogStatus = Status;
        #endif
    }
    else {
        #if REFIT_DEBUG > 0
        // Log 'Access Denied'
        LogStatus = EFI_ACCESS_DENIED;
        #endif

        // Return 'Success'
        Status = EFI_SUCCESS;
    }


    #if REFIT_DEBUG > 0
    CHAR16 *MsgStr = NULL;
    if (BlockCert) {
        MsgStr = PoolPrint (
            L"In Hardware NVRAM ... '%r' to Security Certificate:- '%s'",
            LogStatus,
            VariableName
        );
        LOG(1, LOG_THREE_STAR_MID, L"%s!!", MsgStr);
        MsgLog ("\n");
        MsgLog ("%s", MsgStr);
        MsgLog ("%s  - Successful Write May Result in BootROM Damage", OffsetNext);
    }
    else {
        MsgStr = PoolPrint (
            L"In Hardware NVRAM ... '%r' on Writing Variable:- '%s'",
            LogStatus,
            VariableName
        );
        LOG(1, LOG_THREE_STAR_MID, L"%s", MsgStr);
        MsgLog ("\n");
        MsgLog ("%s", MsgStr);
    }
    MsgLog ("\n");
    MY_FREE_POOL(MsgStr);
    #endif

    return Status;
} // VOID gRTSetVariableEx()

static
VOID FilterCSR (VOID) {
    EFI_STATUS Status;

    if (GlobalConfig.NormaliseCSR) {
        // Filter out the 'APPLE_INTERNAL' CSR bit if present
        Status = NormaliseCSR();

        #if REFIT_DEBUG > 0
        CHAR16 *MsgStr = PoolPrint (
            L"Normalise CSR ... %r",
            Status
        );
        LOG(1, LOG_THREE_STAR_MID, L"%s", MsgStr);
        MsgLog ("%s    * %s", OffsetNext, MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif
    }
} // static VOID FilterCSR()

static
VOID ActiveCSR (VOID) {
    UINT32  CsrStatus;
    BOOLEAN CsrEnabled = FALSE;
    BOOLEAN RotateCsr  = FALSE;

    if ((GlobalConfig.ActiveCSR == 0) ||
        (GlobalConfig.ActiveCSR != -1 && GlobalConfig.ActiveCSR != 1)
    ) {
        // Early return if improperly set or configured not to set CSR
        return;
    }

    // Prime 'Status' for logging
    #if REFIT_DEBUG > 0
    EFI_STATUS Status = EFI_ALREADY_STARTED;
    #endif

    // Try to get current CSR status
    if (GetCsrStatus (&CsrStatus) == EFI_SUCCESS) {
        // Record CSR status in the 'gCsrStatus' variable
        RecordgCsrStatus (CsrStatus, FALSE);

        // Check 'gCsrStatus' variable for 'Enabled' term
        if (MyStrStr (gCsrStatus, L"Enabled")) {
            // 'Enabled' found
            CsrEnabled = TRUE;
        }
        else {
            // 'Enabled' not found
            CsrEnabled = FALSE;
        }

        if (GlobalConfig.ActiveCSR == -1) {
            // Set to always disable
            //
            // Seed the log buffer
            #if REFIT_DEBUG > 0
            MsgLog ("INFO: Disable SIP/SSV ... ");
            #endif

            if (CsrEnabled) {
                // Switch SIP/SSV off as currently enabled
                RotateCsr = TRUE;
            }
        }
        else {
            // Set to always enable ... GlobalConfig.ActiveCSR == 1
            //
            // Seed the log buffer
            #if REFIT_DEBUG > 0
            MsgLog ("INFO: Enable SIP/SSV ... ");
            #endif

            if (!CsrEnabled) {
                // Switch SIP/SSV on as currently disbled
                RotateCsr = TRUE;
            }
        }

        if (RotateCsr) {
            // Toggle SIP/SSV from current setting
            RotateCsrValue ();

            // Set 'Status' to 'Success'
            #if REFIT_DEBUG > 0
            Status = EFI_SUCCESS;
            #endif
        }

        // Finalise and flush the log buffer
        #if REFIT_DEBUG > 0
        MsgLog ("%r\n\n", Status);
        #endif
    }
} // static VOID ActiveCSR()


static
VOID SetBootArgs (VOID) {
    EFI_STATUS   Status     = EFI_NOT_STARTED;
    EFI_GUID     AppleGUID  = APPLE_GUID;
    CHAR16      *NameNVRAM  = L"boot-args";
    CHAR16      *BootArg    = NULL;
    CHAR8       *DataNVRAM  = NULL;

    #if REFIT_DEBUG > 0
    CHAR16  *MsgStr                = NULL;
    BOOLEAN  LogDisableAMFI        = FALSE;
    BOOLEAN  LogDisableCompatCheck = FALSE;
    #endif

    if (!GlobalConfig.SetBootArgs || GlobalConfig.SetBootArgs[0] == L'\0') {
        #if REFIT_DEBUG > 0
        Status = EFI_INVALID_PARAMETER;
        #endif
    }
    else {
        if (MyStrStr (GlobalConfig.SetBootArgs, L"amfi_get_out_of_my_way=1")) {
            #if REFIT_DEBUG > 0
            if (GlobalConfig.DisableAMFI) {
                // Ensure Logging
                LogDisableAMFI = TRUE;
            }
            #endif

            // Do not duplicate 'amfi_get_out_of_my_way=1'
            GlobalConfig.DisableAMFI = FALSE;
        }

        if (MyStrStr (GlobalConfig.SetBootArgs, L"-no_compat_check")) {
            #if REFIT_DEBUG > 0
            if (GlobalConfig.DisableCompatCheck) {
                // Ensure Logging
                LogDisableCompatCheck = TRUE;
            }
            #endif

            // Do not duplicate '-no_compat_check'
            GlobalConfig.DisableCompatCheck = FALSE;
        }

        if (GlobalConfig.DisableAMFI &&
            GlobalConfig.DisableCompatCheck
        ) {
            // Combine Args with DisableAMFI and DisableAMFI
            BootArg = PoolPrint (
                L"%s amfi_get_out_of_my_way=1 -no_compat_check",
                GlobalConfig.SetBootArgs
            );
        }
        else if (GlobalConfig.DisableAMFI) {
            // Combine Args with DisableAMFI
            BootArg = PoolPrint (
                L"%s amfi_get_out_of_my_way=1",
                GlobalConfig.SetBootArgs
            );
        }
        else if (GlobalConfig.DisableCompatCheck) {
            // Combine Args with DisableCompatCheck
            BootArg = PoolPrint (
                L"%s -no_compat_check",
                GlobalConfig.SetBootArgs
            );
        }
        else {
            // Use Args Alone
            BootArg = PoolPrint (L"%s", GlobalConfig.SetBootArgs);
        }

        if (BootArg) {
            DataNVRAM = AllocateZeroPool (
                (StrLen (BootArg) + 1) * sizeof (CHAR8)
            );

            if (DataNVRAM) {
                // Convert Unicode String 'BootArg' to Ascii String 'DataNVRAM'
                UnicodeStrToAsciiStr (BootArg, DataNVRAM);

                Status = EfivarSetRaw (
                    &AppleGUID,
                    NameNVRAM,
                    DataNVRAM,
                    AsciiStrSize (DataNVRAM),
                    TRUE
                );
                MY_FREE_POOL(DataNVRAM);
            }
            MY_FREE_POOL(BootArg);
        }
    }

    #if REFIT_DEBUG > 0
    if (LogDisableAMFI || GlobalConfig.DisableAMFI) {
        MsgStr = PoolPrint (
            L"Disable AMFI ... %r",
            Status
        );
        LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
        MsgLog ("%s    * %s", OffsetNext, MsgStr);
        MY_FREE_POOL(MsgStr);
    }

    MsgStr = PoolPrint (
        L"Reset Boot Args ... %r",
        Status
    );
    LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
    MsgLog ("%s    * %s", OffsetNext, MsgStr);
    MY_FREE_POOL(MsgStr);

    if (LogDisableCompatCheck || GlobalConfig.DisableCompatCheck) {
        MsgStr = PoolPrint (
            L"Disable Compat Check ... %r",
            Status
        );
        LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
        MsgLog ("%s    * %s", OffsetNext, MsgStr);
        MY_FREE_POOL(MsgStr);
    }
    #endif
} // static VOID SetBootArgs()


VOID DisableAMFI (VOID) {
    EFI_STATUS   Status;
    EFI_GUID     AppleGUID  = APPLE_GUID;
    CHAR16      *NameNVRAM  = L"boot-args";

    #if REFIT_DEBUG > 0
    CHAR16  *MsgStr = NULL;
    #endif

    if (GlobalConfig.DisableCompatCheck) {
        // Combine with DisableCompatCheck
        CHAR8 *DataNVRAM = "amfi_get_out_of_my_way=1 -no_compat_check";

        Status = EfivarSetRaw (
            &AppleGUID,
            NameNVRAM,
            DataNVRAM,
            AsciiStrSize (DataNVRAM),
            TRUE
        );
    }
    else {
        CHAR8 *DataNVRAM = "amfi_get_out_of_my_way=1";

        Status = EfivarSetRaw (
            &AppleGUID,
            NameNVRAM,
            DataNVRAM,
            AsciiStrSize (DataNVRAM),
            TRUE
        );
    }

    #if REFIT_DEBUG > 0
    MsgStr = PoolPrint (
        L"Disable AMFI ... %r",
        Status
    );
    LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
    MsgLog ("%s    * %s", OffsetNext, MsgStr);
    MY_FREE_POOL(MsgStr);

    if (GlobalConfig.DisableCompatCheck) {
        MsgStr = PoolPrint (
            L"Disable Compat Check ... %r",
            Status
        );
        LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
        MsgLog ("%s    * %s", OffsetNext, MsgStr);
        MY_FREE_POOL(MsgStr);
    }
    #endif
} // VOID DisableAMFI()


VOID DisableCompatCheck (VOID) {
    EFI_STATUS   Status;
    EFI_GUID     AppleGUID  = APPLE_GUID;
    CHAR16      *NameNVRAM  = L"boot-args";
    CHAR8       *DataNVRAM  = "-no_compat_check";

    #if REFIT_DEBUG > 0
    CHAR16  *MsgStr = NULL;
    #endif

    Status = EfivarSetRaw (
        &AppleGUID,
        NameNVRAM,
        DataNVRAM,
        AsciiStrSize (DataNVRAM),
        TRUE
    );

    #if REFIT_DEBUG > 0
    MsgStr = PoolPrint (
        L"Disable Compat Check ... %r",
        Status
    );
    LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
    MsgLog ("%s    * %s", OffsetNext, MsgStr);
    MY_FREE_POOL(MsgStr);
    #endif
} // VOID DisableCompatCheck()


VOID ForceTRIM (VOID) {
    EFI_STATUS   Status;
    EFI_GUID     AppleGUID     = APPLE_GUID;
    CHAR16      *NameNVRAM     = L"EnableTRIM";
    CHAR8        DataNVRAM[1]  = {0x01};

    #if REFIT_DEBUG > 0
    CHAR16  *MsgStr = NULL;
    #endif

    Status = EfivarSetRaw (
        &AppleGUID,
        NameNVRAM,
        DataNVRAM,
        sizeof (DataNVRAM),
        TRUE
    );

    #if REFIT_DEBUG > 0
    MsgStr = PoolPrint (
        L"Forcibly Enable TRIM ... %r",
        Status
    );
    LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
    MsgLog ("%s    * %s", OffsetNext, MsgStr);
    MY_FREE_POOL(MsgStr);
    #endif
} // VOID ForceTRIM()


// Extended 'OpenProtocol'
// Ensures GOP Interface for Boot Loading Screen
static
EFI_STATUS EFIAPI OpenProtocolEx (
    IN   EFI_HANDLE    Handle,
    IN   EFI_GUID     *Protocol,
    OUT  VOID        **Interface OPTIONAL,
    IN   EFI_HANDLE    AgentHandle,
    IN   EFI_HANDLE    ControllerHandle,
    IN   UINT32        Attributes
) {
    EFI_STATUS   Status;
    UINTN        i              = 0;
    UINTN        HandleCount    = 0;
    EFI_HANDLE  *HandleBuffer   = NULL;

    Status = OrigOpenProtocol (
        Handle,
        Protocol,
        Interface,
        AgentHandle,
        ControllerHandle,
        Attributes
    );

    if (Status == EFI_UNSUPPORTED) {
        if (GuidsAreEqual (&gEfiGraphicsOutputProtocolGuid, Protocol)) {
            if (GOPDraw != NULL) {
                Status = EFI_SUCCESS;
                *Interface = GOPDraw;
            }
            else {
                Status = REFIT_CALL_5_WRAPPER(
                    gBS->LocateHandleBuffer,
                    ByProtocol,
                    &gEfiGraphicsOutputProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                );

                if (!EFI_ERROR(Status)) {
                    for (i = 0; i < HandleCount; i++) {
                        if (HandleBuffer[i] != gST->ConsoleOutHandle) {
                            Status = REFIT_CALL_6_WRAPPER(
                                OrigOpenProtocol,
                                HandleBuffer[i],
                                &gEfiGraphicsOutputProtocolGuid,
                                *Interface,
                                AgentHandle,
                                NULL,
                                EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                            );

                            if (!EFI_ERROR(Status)) {
                                break;
                            }
                        }
                    } // for
                } // if !EFI_ERROR Status

                if (EFI_ERROR(Status) || *Interface == NULL) {
                    Status = EFI_UNSUPPORTED;
                }
            } // If GOPDraw != NULL
        } // if GuidsAreEqual

        MY_FREE_POOL(HandleBuffer);
    } // if Status == EFI_UNSUPPORTED

    return Status;
} // EFI_STATUS OpenProtocolEx()


// Extended 'HandleProtocol'
// Routes 'HandleProtocol' to 'OpenProtocol'
static
EFI_STATUS EFIAPI HandleProtocolEx (
    IN   EFI_HANDLE   Handle,
    IN   EFI_GUID    *Protocol,
    OUT  VOID       **Interface
) {
    EFI_STATUS Status;

    Status = REFIT_CALL_6_WRAPPER(
        gBS->OpenProtocol,
        Handle,
        Protocol,
        Interface,
        gImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
    );

    return Status;
} // EFI_STATUS HandleProtocolEx()

static
VOID ReMapOpenProtocol (VOID) {
    // Amend EFI_BOOT_SERVICES.OpenProtocol
    OrigOpenProtocol    = gBS->OpenProtocol;
    gBS->OpenProtocol   = OpenProtocolEx;
    gBS->Hdr.CRC32      = 0;
    gBS->CalculateCrc32 (gBS, gBS->Hdr.HeaderSize, &gBS->Hdr.CRC32);
} // ReMapOpenProtocol()


VOID preBootKicker (VOID) {
    CHAR16 *ToolPurpose = L"Kick in the Apple Boot Screen";

    REFIT_MENU_SCREEN *PreBootKickerMenu = AllocateZeroPool (sizeof (REFIT_MENU_SCREEN));
    if (PreBootKickerMenu) {
        PreBootKickerMenu->TitleImage = BuiltinIcon (BUILTIN_ICON_TOOL_BOOTKICKER);
        PreBootKickerMenu->Title      = StrDuplicate (L"BootKicker");
        PreBootKickerMenu->Hint1      = StrDuplicate (L"Press 'ESC', 'BackSpace' or 'SpaceBar' to Return to Main Menu");
        PreBootKickerMenu->Hint2      = StrDuplicate (L"");

        AddMenuInfoLineAlt (PreBootKickerMenu, PoolPrint (L"A Tool to %s", ToolPurpose));
        AddMenuInfoLine (PreBootKickerMenu, L"Needs GOP Capable Fully Compatible GPUs on Apple Firmware");
        AddMenuInfoLine (PreBootKickerMenu, L"(Fully Compatible GPUs provide native Apple Boot Screen)");
        AddMenuInfoLine (PreBootKickerMenu, L"NB: Hangs and needs physical reboot with other GPUs");
        AddMenuInfoLine (PreBootKickerMenu, L"");
        AddMenuInfoLine (PreBootKickerMenu, L"BootKicker is from OpenCore and is Copyright Acidanthera");
        AddMenuInfoLine (PreBootKickerMenu, L"You can find copies of the binary file here:");
        AddMenuInfoLine (PreBootKickerMenu, L"1) https://github.com/acidanthera/OpenCorePkg/releases");
        AddMenuInfoLine (PreBootKickerMenu, L"2) https://github.com/dakanji/RefindPlus/tree/GOPFix/BootMaster/tools_x64");
        AddMenuInfoLine (PreBootKickerMenu, L"");
        AddMenuInfoLine (PreBootKickerMenu, L"Requires at least one of the files below.");
        AddMenuInfoLine (PreBootKickerMenu, L" - The first file found in the order listed will be used");
        AddMenuInfoLine (PreBootKickerMenu, L" - You will be returned to the main menu if not found");
        AddMenuInfoLine (PreBootKickerMenu, L"");

        UINTN k = 0;
        CHAR16 *FilePath = NULL;
        while ((FilePath = FindCommaDelimited (BOOTKICKER_FILES, k++)) != NULL) {
            AddMenuInfoLineAlt (PreBootKickerMenu, StrDuplicate (FilePath));
            MY_FREE_POOL(FilePath);
        }

        AddMenuInfoLine (PreBootKickerMenu, L"");

        REFIT_MENU_ENTRY *MenuEntryBootKicker = AllocateZeroPool (sizeof (REFIT_MENU_ENTRY));
        MenuEntryBootKicker->Title = StrDuplicate (ToolPurpose);
        MenuEntryBootKicker->Tag   = TAG_RETURN;
        MenuEntryBootKicker->Row   = 1;
        AddMenuEntry (PreBootKickerMenu, MenuEntryBootKicker);

        REFIT_MENU_ENTRY *LocalMenuEntryReturn = AllocateZeroPool (sizeof (REFIT_MENU_ENTRY));
        LocalMenuEntryReturn->Title = StrDuplicate (L"Return to Main Menu");
        LocalMenuEntryReturn->Tag   = TAG_RETURN;
        LocalMenuEntryReturn->Row   = 1;
        AddMenuEntry (PreBootKickerMenu, LocalMenuEntryReturn);

        INTN               DefaultEntry   = 1;
        MENU_STYLE_FUNC    Style          = GraphicsMenuStyle;
        REFIT_MENU_ENTRY  *ChosenEntry    = NULL;
        UINTN MenuExit = RunGenericMenu (PreBootKickerMenu, Style, &DefaultEntry, &ChosenEntry);

        #if REFIT_DEBUG > 0
        LOG(1, LOG_LINE_NORMAL,
            L"Returned '%d' (%s) from RunGenericMenu Call on '%s' in 'preBootKicker'",
            MenuExit, MenuExitInfo (MenuExit), ChosenEntry->Title
        );
        MsgLog ("User Input Received:");
        MsgLog ("%s  - %s", OffsetNext, ChosenEntry->Title);
        #endif

        if (ChosenEntry->Tag != TAG_LOAD_BOOTKICKER || MenuExit != MENU_EXIT_ENTER) {
            #if REFIT_DEBUG > 0
            // Return to Main Screen
            MsgLog ("\n\n");
            #endif
        }
        else {
            k = 0;
            LOADER_ENTRY *ourLoaderEntry = NULL;
            BOOLEAN FoundTool = FALSE;
            while (!FoundTool && (FilePath = FindCommaDelimited (BOOTKICKER_FILES, k++)) != NULL) {
                #if REFIT_DEBUG > 0
                MsgLog ("%s    * Seek %s:", OffsetNext, FilePath);
                #endif

                for (UINTN i = 0; i < VolumesCount; i++) {
                    if (Volumes[i]->RootDir != NULL &&
                        FileExists (Volumes[i]->RootDir, FilePath)
                    ) {
                        ourLoaderEntry = AllocateZeroPool (sizeof (LOADER_ENTRY));
                        ourLoaderEntry->me.Title          = StrDuplicate (ChosenEntry->Title);
                        ourLoaderEntry->me.Tag            = TAG_LOAD_BOOTKICKER;
                        ourLoaderEntry->me.Row            = 1;
                        ourLoaderEntry->me.ShortcutLetter = 0;
                        ourLoaderEntry->me.Image          = BuiltinIcon (BUILTIN_ICON_TOOL_BOOTKICKER);
                        ourLoaderEntry->LoaderPath        = StrDuplicate (FilePath);
                        ourLoaderEntry->Volume            = CopyVolume (Volumes[i]);
                        ourLoaderEntry->UseGraphicsMode   = TRUE;

                        FoundTool = TRUE;

                        // Break out of 'for' loop
                        break;
                    }
                } // for

                if (!FoundTool) {
                    MY_FREE_POOL(FilePath);
                }
            } // while

            #if REFIT_DEBUG > 0
            MsgLog ("\n");
            LOG(1, LOG_BLANK_LINE_SEP, L"X");
            #endif

            if (!FoundTool) {
                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"'Not Found' When Locating BootKicker Tool:- '%s'", FilePath);
                MsgLog ("\n");
                MsgLog ("** WARN ** Could Not Find BootKicker ... Return to Main Menu\n\n");
                #endif

                MY_FREE_POOL(FilePath);
            }
            else {
                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"'Success' When Locating BootKicker Tool:- '%s'", FilePath);
                MsgLog ("%s    ** 'Success' When Locating:- '%s'", OffsetNext, FilePath);
                MsgLog ("%s  - Load BootKicker\n\n", OffsetNext);
                #endif

                MY_FREE_POOL(FilePath);

                // Run BootKicker
                StartTool (ourLoaderEntry);

                // If we get here, an error was met while starting the tool
                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"Run BootKicker Error ... Return to Main Menu");
                MsgLog ("** WARN ** BootKicker Error ... Return to Main Menu\n\n");
                #endif

                FreeLoaderEntry (&ourLoaderEntry);
            }
        } // if !MyStriCmp ChosenEntry->Title

        FreeMenuScreen (&PreBootKickerMenu);
    } // if PreBootKickerMenu
} // VOID preBootKicker()

static
VOID preCleanNvram (VOID) {
    CHAR16 *ToolPurpose = L"Clean/Reset NVRAM";

    REFIT_MENU_SCREEN *PreCleanNvramMenu = AllocateZeroPool (sizeof (REFIT_MENU_SCREEN));
    if (PreCleanNvramMenu) {
        PreCleanNvramMenu->TitleImage = BuiltinIcon (BUILTIN_ICON_TOOL_NVRAMCLEAN);
        PreCleanNvramMenu->Title      = StrDuplicate (L"Clean/Reset NVRAM");
        PreCleanNvramMenu->Hint1      = StrDuplicate (L"Press 'ESC', 'BackSpace' or 'SpaceBar' to Return to Main Menu");
        PreCleanNvramMenu->Hint2      = StrDuplicate (L"");

        AddMenuInfoLineAlt (PreCleanNvramMenu, PoolPrint (L"A Tool to %s", ToolPurpose));
        AddMenuInfoLine (PreCleanNvramMenu, L"");
        AddMenuInfoLine (PreCleanNvramMenu, L"CleanNvram is from OpenCore and is Copyright Acidanthera");
        AddMenuInfoLine (PreCleanNvramMenu, L"You can find copies of the binary file here:");
        AddMenuInfoLine (PreCleanNvramMenu, L"1) https://github.com/acidanthera/OpenCorePkg/releases");
        AddMenuInfoLine (PreCleanNvramMenu, L"2) https://github.com/dakanji/RefindPlus/tree/GOPFix/BootMaster/tools_x64");
        AddMenuInfoLine (PreCleanNvramMenu, L"");
        AddMenuInfoLine (PreCleanNvramMenu, L"Requires at least one of the files below.");
        AddMenuInfoLine (PreCleanNvramMenu, L" - The first file found in the order listed will be used");
        AddMenuInfoLine (PreCleanNvramMenu, L" - You will be returned to the main menu if not found");
        AddMenuInfoLine (PreCleanNvramMenu, L"");

        UINTN k = 0;
        CHAR16 *FilePath = NULL;
        while ((FilePath = FindCommaDelimited (NVRAMCLEAN_FILES, k++)) != NULL) {
            AddMenuInfoLineAlt (PreCleanNvramMenu, StrDuplicate (FilePath));
            MY_FREE_POOL(FilePath);
        }

        AddMenuInfoLine (PreCleanNvramMenu, L"");

        REFIT_MENU_ENTRY *MenuEntryCleanNvram = AllocateZeroPool (sizeof (REFIT_MENU_ENTRY));
        MenuEntryCleanNvram->Title = StrDuplicate (ToolPurpose);
        MenuEntryCleanNvram->Tag   = TAG_RETURN;
        MenuEntryCleanNvram->Row   = 1;
        AddMenuEntry (PreCleanNvramMenu, MenuEntryCleanNvram);

        REFIT_MENU_ENTRY *LocalMenuEntryReturn = AllocateZeroPool (sizeof (REFIT_MENU_ENTRY));
        LocalMenuEntryReturn->Title = StrDuplicate (L"Return to Main Menu");
        LocalMenuEntryReturn->Tag   = TAG_RETURN;
        LocalMenuEntryReturn->Row   = 1;
        AddMenuEntry (PreCleanNvramMenu, LocalMenuEntryReturn);

        INTN               DefaultEntry   = 1;
        MENU_STYLE_FUNC    Style          = GraphicsMenuStyle;
        REFIT_MENU_ENTRY  *ChosenEntry    = NULL;
        UINTN MenuExit = RunGenericMenu (PreCleanNvramMenu, Style, &DefaultEntry, &ChosenEntry);

        #if REFIT_DEBUG > 0
        LOG(1, LOG_LINE_NORMAL,
            L"Returned '%d' (%s) from RunGenericMenu Call on '%s' in 'preCleanNvram'",
            MenuExit, MenuExitInfo (MenuExit), ChosenEntry->Title
        );
        MsgLog ("User Input Received:");
        MsgLog ("%s  - %s", OffsetNext, ChosenEntry->Title);
        #endif

        if (ChosenEntry->Tag != TAG_LOAD_NVRAMCLEAN || MenuExit != MENU_EXIT_ENTER) {
            #if REFIT_DEBUG > 0
            // Return to Main Screen
            MsgLog ("\n\n");
            #endif
        }
        else {
            k = 0;
            LOADER_ENTRY *ourLoaderEntry = NULL;
            BOOLEAN FoundTool = FALSE;
            while (!FoundTool && (FilePath = FindCommaDelimited (NVRAMCLEAN_FILES, k++)) != NULL) {
                #if REFIT_DEBUG > 0
                MsgLog ("%s    * Seek %s:", OffsetNext, FilePath);
                #endif

                for (UINTN i = 0; i < VolumesCount; i++) {
                    if (Volumes[i]->RootDir != NULL &&
                        FileExists (Volumes[i]->RootDir, FilePath)
                    ) {
                        ourLoaderEntry = AllocateZeroPool (sizeof (LOADER_ENTRY));
                        ourLoaderEntry->me.Title          = StrDuplicate (ChosenEntry->Title);
                        ourLoaderEntry->me.Tag            = TAG_LOAD_NVRAMCLEAN;
                        ourLoaderEntry->me.Row            = 1;
                        ourLoaderEntry->me.ShortcutLetter = 0;
                        ourLoaderEntry->me.Image          = BuiltinIcon (BUILTIN_ICON_TOOL_NVRAMCLEAN);
                        ourLoaderEntry->LoaderPath        = StrDuplicate (FilePath);
                        ourLoaderEntry->Volume            = CopyVolume (Volumes[i]);
                        ourLoaderEntry->UseGraphicsMode   = TRUE;

                        FoundTool = TRUE;

                        // Break out of 'for' loop
                        break;
                    }
                } // for

                if (!FoundTool) {
                    MY_FREE_POOL(FilePath);
                }
            } // while

            #if REFIT_DEBUG > 0
            MsgLog ("\n");
            LOG(1, LOG_BLANK_LINE_SEP, L"X");
            #endif

            if (!FoundTool) {
                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"'Not Found' When Locating CleanNvram Tool:- '%s'", FilePath);
                MsgLog ("\n");
                MsgLog ("** WARN ** Could Not Find CleanNvram ... Return to Main Menu\n\n");
                #endif

                MY_FREE_POOL(FilePath);
            }
            else {
                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"'Success' When Locating CleanNvram Tool:- '%s'", FilePath);
                MsgLog ("%s    ** 'Success' When Locating:- '%s'", OffsetNext, FilePath);
                MsgLog ("%s  - Load CleanNvram\n\n", OffsetNext);
                #endif

                MY_FREE_POOL(FilePath);

                ranCleanNvram = TRUE;

                // Run CleanNvram
                StartTool (ourLoaderEntry);

                // If we get here, an error was met while starting the tool
                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"Run CleanNvram Error ... Return to Main Menu");
                MsgLog ("** WARN ** CleanNvram Error ... Return to Main Menu\n\n");
                #endif

                FreeLoaderEntry (&ourLoaderEntry);
            }
        } // if !MyStriCmp ChosenEntry->Title

        FreeMenuScreen (&PreCleanNvramMenu);
    } // if PreCleanNvramMenu
} // VOID preCleanNvram()


VOID AboutRefindPlus (VOID) {
    REFIT_MENU_SCREEN *AboutMenu = AllocateZeroPool (sizeof (REFIT_MENU_SCREEN));

    if (AboutMenu) {
        #if REFIT_DEBUG > 0
        LOG(1, LOG_LINE_THIN_SEP, L"Creating 'About RefindPlus' Screen");
        #endif

        UINT32   CsrStatus;
        CHAR16  *VendorString = StrDuplicate (VendorInfo);

        // More than ~65 causes empty info page on 800x600 display
        LimitStringLength (VendorString, MAX_LINE_LENGTH);

        AboutMenu->Title      = StrDuplicate (L"About RefindPlus");
        AboutMenu->TitleImage = BuiltinIcon (BUILTIN_ICON_FUNC_ABOUT);
        AboutMenu->Hint1      = StrDuplicate (L"Press 'Enter' to Return to Main Menu");
        AboutMenu->Hint2      = StrDuplicate (L"");

        AboutMenu->InfoLines  = (CHAR16 **) AllocateZeroPool (sizeof (CHAR16 *));

        AddMenuInfoLineAlt (
            AboutMenu,
            PoolPrint (
                L"RefindPlus v%s",
                REFINDPLUS_VERSION
            )
        );

        AddMenuInfoLine (AboutMenu, L"");
        AddMenuInfoLine (AboutMenu, L"Copyright (c) 2020-2021 Dayo Akanji and Others");
        AddMenuInfoLine (AboutMenu, L"Portions Copyright (c) 2012-2021 Roderick W. Smith");
        AddMenuInfoLine (AboutMenu, L"Portions Copyright (c) 2006-2010 Christoph Pfisterer");
        AddMenuInfoLine (AboutMenu, L"Portions Copyright (c) The Intel Corporation and others");
        AddMenuInfoLine (AboutMenu, L"Distributed under the terms of the GNU GPLv3 license");
        AddMenuInfoLine (AboutMenu, L"");

        #if defined (__MAKEWITH_GNUEFI)
        AddMenuInfoLine (AboutMenu, L"Built with GNU-EFI");
        #else
        AddMenuInfoLine (AboutMenu, L"Built with TianoCore EDK II");
        #endif

        AddMenuInfoLine (AboutMenu, L"");

        AddMenuInfoLineAlt (
            AboutMenu,
            PoolPrint (
                L"Firmware Vendor: %s",
                VendorString
            )
        );
        MY_FREE_POOL(VendorString);

        #if defined (EFI32)
        AddMenuInfoLine (AboutMenu, L"Platform: x86 (32 bit)");
        #elif defined (EFIX64)
        AddMenuInfoLine (AboutMenu, L"Platform: x86_64 (64 bit)");
        #elif defined (EFIAARCH64)
        AddMenuInfoLine (AboutMenu, L"Platform: ARM (64 bit)");
        #else
        AddMenuInfoLine (AboutMenu, L"Platform: Unknown");
        #endif

        AddMenuInfoLineAlt (
            AboutMenu,
            PoolPrint (
                L"EFI Revision: %s %d.%02d",
                ((gST->Hdr.Revision >> 16) == 1) ? L"EFI" : L"UEFI",
                gST->Hdr.Revision >> 16,
                gST->Hdr.Revision & ((1 << 16) - 1)
            )
        );
        AddMenuInfoLineAlt (
            AboutMenu,
            PoolPrint (
                L"Secure Boot: %s",
                secure_mode() ? L"Active" : L"Inactive"
            )
        );

        if (GetCsrStatus (&CsrStatus) == EFI_SUCCESS) {
            RecordgCsrStatus (CsrStatus, FALSE);
            AddMenuInfoLine (AboutMenu, gCsrStatus);
        }

        AddMenuInfoLineAlt (
            AboutMenu,
            PoolPrint(
                L"Screen Output: %s",
                egScreenDescription()
            )
        );

        AddMenuInfoLine (AboutMenu, L"");
        AddMenuInfoLine (AboutMenu, L"RefindPlus is a variant of rEFInd");
        AddMenuInfoLine (AboutMenu, L"https://github.com/dakanji/RefindPlus");
        AddMenuInfoLine (AboutMenu, L"");
        AddMenuInfoLine (AboutMenu, L"For information on rEFInd, visit:");
        AddMenuInfoLine (AboutMenu, L"http://www.rodsbooks.com/refind");

        REFIT_MENU_ENTRY *LocalMenuEntryReturn = AllocateZeroPool (sizeof (REFIT_MENU_ENTRY));
        LocalMenuEntryReturn->Title = StrDuplicate (L"Return to Main Menu");
        LocalMenuEntryReturn->Tag   = TAG_RETURN;
        LocalMenuEntryReturn->Row   = 1;
        AddMenuEntry (AboutMenu, LocalMenuEntryReturn);

        RunMenu (AboutMenu, NULL);
        FreeMenuScreen (&AboutMenu);
    } // if (AboutMenu)
} // VOID AboutRefindPlus()

// Record the loader's name/description in the "PreviousBoot" UEFI variable
// if different from what is already stored there.
VOID StoreLoaderName (
    IN CHAR16 *Name
) {
    // Do not set if configured not to
    if (GlobalConfig.IgnorePreviousBoot) {
        return;
    }

    if (Name) {
        EfivarSetRaw (
            &RefindPlusGuid,
            L"PreviousBoot",
            Name,
            StrLen (Name) * 2 + 2,
            TRUE
        );
    }
} // VOID StoreLoaderName()

// Rescan for boot loaders
VOID RescanAll (
    BOOLEAN DisplayMessage,
    BOOLEAN Reconnect
) {
    #if REFIT_DEBUG > 0
    CHAR16 *MsgStr = L"Repeat Tool/Loader Scan";

    LOG(1, LOG_STAR_SEPARATOR, L"%s", MsgStr);
    MsgLog ("INFO: %s\n\n", MsgStr);
    #endif

    /* Enable Forced Native Logging */
    NativeLogger = TRUE;

    // Reset MainMenu
    InitMainMenu();

    // ConnectAllDriversToAllControllers() can cause system hangs with some
    // buggy filesystem drivers, so do it only if necessary.
    if (Reconnect) {
        ConnectAllDriversToAllControllers (FALSE);
        ScanVolumes();
    }

    ReadConfig (GlobalConfig.ConfigFilename);
    SetVolumeIcons();
    ScanForBootloaders (DisplayMessage);
    ScanForTools();

    /* Disable Forced Native Logging */
    NativeLogger = FALSE;
} // VOID RescanAll()

#ifdef __MAKEWITH_TIANO
// Minimal initialisation function
static
VOID InitializeLib (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
) {
    gImageHandle  = ImageHandle;
    gST           = SystemTable;
    gBS           = SystemTable->BootServices;
    gRT           = SystemTable->RuntimeServices;

    EfiGetSystemConfigurationTable (
        &gEfiDxeServicesTableGuid,
        (VOID **) &gDS
    );

    // Upgrade EFI_BOOT_SERVICES.HandleProtocol
    OrigHandleProtocol   = gBS->HandleProtocol;
    gBS->HandleProtocol  = HandleProtocolEx;
    gBS->Hdr.CRC32       = 0;
    gBS->CalculateCrc32 (gBS, gBS->Hdr.HeaderSize, &gBS->Hdr.CRC32);
} // VOID InitializeLib()
#endif

// Set up our own Secure Boot extensions.
// Returns TRUE on success, FALSE otherwise
static
BOOLEAN SecureBootSetup (VOID) {
    if (secure_mode() && ShimLoaded()) {
        if (security_policy_install() == EFI_SUCCESS) {
            return TRUE;
        }

        SecureBootFailure = TRUE;

        #if REFIT_DEBUG > 0
        MsgLog ("** FATAL ERROR: Failed to Install MOK Secure Boot Extensions");
        MsgLog ("\n\n");
        #endif
    }

    return FALSE;
} // VOID SecureBootSetup()

// Remove our own Secure Boot extensions.
// Returns TRUE on success, FALSE otherwise
static
BOOLEAN SecureBootUninstall (VOID) {
    EFI_STATUS  Status;
    BOOLEAN     Success = TRUE;

    if (secure_mode()) {
        Status = security_policy_uninstall();
        if (Status != EFI_SUCCESS) {
            Success = FALSE;
            BeginTextScreen (L"Secure Boot Policy Failure");

            CHAR16 *MsgStr = L"Failed to Uninstall MOK Secure Boot Extensions ... Forcing Shutdown in 9 Seconds";

            #if REFIT_DEBUG > 0
            MsgLog ("%s\n-----------------\n\n", MsgStr);
            #endif

            MuteLogger = TRUE;
            REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_ERROR);
            PrintUglyText (MsgStr, NEXTLINE);
            REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_BASIC);
            MuteLogger = FALSE;

            PauseSeconds (9);

            REFIT_CALL_4_WRAPPER(
                gRT->ResetSystem,
                EfiResetShutdown,
                EFI_SUCCESS,
                0, NULL
            );
        }
    }

    return Success;
} // BOOLEAN SecureBootUninstall()

// Sets the global configuration filename; will be CONFIG_FILE_NAME unless the
// "-c" command-line option is set, in which case that takes precedence.
// If an error is encountered, leaves the value alone (it should be set to
// CONFIG_FILE_NAME when GlobalConfig is initialized).
static
VOID SetConfigFilename (
    EFI_HANDLE ImageHandle
) {
    EFI_STATUS         Status;
    CHAR16            *Options;
    CHAR16            *FileName;
    CHAR16            *SubString;
    CHAR16            *MsgStr = NULL;
    EFI_LOADED_IMAGE  *Info;

    Status = REFIT_CALL_3_WRAPPER(
        gBS->HandleProtocol,
        ImageHandle,
        &LoadedImageProtocol,
        (VOID **) &Info
    );

    if ((Status == EFI_SUCCESS) && (Info->LoadOptionsSize > 0)) {
        Options   = (CHAR16 *) Info->LoadOptions;
        SubString = MyStrStr (Options, L" -c ");
        if (SubString) {
            #if REFIT_DEBUG > 0
            MsgLog ("Set Config Filename from Command Line Option:\n");
            #endif

            FileName = StrDuplicate (&SubString[4]);

            if (FileName) {
                LimitStringLength (FileName, 256);
            }

            if (FileExists (SelfDir, FileName)) {
                GlobalConfig.ConfigFilename = FileName;

                #if REFIT_DEBUG > 0
                MsgLog ("  - Config File:- '%s'\n\n", FileName);
                #endif
            }
            else {
                MsgStr = StrDuplicate (L"** WARN: Specified Config File Not Found");
                #if REFIT_DEBUG > 0
                MsgLog ("%s\n", MsgStr);
                #endif

                MuteLogger = TRUE;
                REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_ERROR);
                PrintUglyText (MsgStr, NEXTLINE);
                MuteLogger = FALSE;

                MY_FREE_POOL(MsgStr);

                MsgStr = StrDuplicate (L"         Try Default:- 'config.conf / refind.conf'");
                #if REFIT_DEBUG > 0
                MsgLog ("%s\n\n", MsgStr);
                #endif

                MuteLogger = TRUE;
                PrintUglyText (MsgStr, NEXTLINE);
                REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_BASIC);
                MuteLogger = FALSE;

                PauseSeconds (9);
                MY_FREE_POOL(MsgStr);
            } // if/else FileExists (SelfDir, FileName

            MY_FREE_POOL(FileName);
        } // if SubString
    } // if (Status == EFI_SUCCESS) && Info->LoadOptionsSize
} // static VOID SetConfigFilename()

// Adjust the GlobalConfig.DefaultSelection variable: Replace all "+" elements with the
//  PreviousBoot variable, if it is available. If it is not available, delete that element.
static
VOID AdjustDefaultSelection (VOID) {
    EFI_STATUS  Status;
    UINTN       i                 = 0;
    CHAR16     *Element           = NULL;
    CHAR16     *NewCommaDelimited = NULL;
    CHAR16     *PreviousBoot      = NULL;
    BOOLEAN     Ignore;

    #if REFIT_DEBUG > 0
    MsgLog ("Adjust Default Selection...");
    MsgLog ("\n\n");
    #endif

    while ((Element = FindCommaDelimited (
        GlobalConfig.DefaultSelection, i++
    )) != NULL) {
        Ignore = FALSE;

        if (MyStriCmp (Element, L"+")) {
            if (GlobalConfig.IgnorePreviousBoot &&
                StrLen (GlobalConfig.DefaultSelection) > 1
            ) {
                Ignore = TRUE;
            }

            if (!Ignore) {
                Status = EfivarGetRaw (
                    &RefindPlusGuid,
                    L"PreviousBoot",
                    (VOID **) &PreviousBoot,
                    NULL
                );

                MY_FREE_POOL(Element);
                if (Status == EFI_SUCCESS) {
                    Element = PreviousBoot;
                }
            }
        }

        if (!Ignore && Element && StrLen (Element)) {
            MergeStrings (&NewCommaDelimited, Element, L',');
        }

        MY_FREE_POOL(Element);
    } // while

    MY_FREE_POOL(GlobalConfig.DefaultSelection);
    GlobalConfig.DefaultSelection = NewCommaDelimited;
} // AdjustDefaultSelection()

#if REFIT_DEBUG > 0
static
VOID LogRevisionInfo (
    EFI_TABLE_HEADER *Hdr,
    CHAR16           *Name,
    UINT16            ExpectedSize,
    BOOLEAN           DoEFICheck
) {
    static BOOLEAN FirstRun = TRUE;

    (FirstRun) ? MsgLog ("\n\n") : MsgLog ("\n");
    FirstRun = FALSE;

    MsgLog (
        "%s:- '%-4s %d.%02d'",
        Name,
        DoEFICheck ? ((Hdr->Revision >> 16 == 1) ? L"EFI" : L"UEFI") : L"Ver",
        Hdr->Revision >> 16,
        Hdr->Revision & 0xffff
    );
    if (Hdr->HeaderSize == ExpectedSize) {
        MsgLog (" (HeaderSize: %d)", Hdr->HeaderSize);
    }
    else {
        MsgLog (" (HeaderSize: %d ... Expected: %d)", Hdr->HeaderSize, ExpectedSize);
    }
} // static VOID LogRevisionInfo()
#endif

// Log basic information (RefindPlus version, EFI version, etc.) to the log file.
// Also sets some variables that may be needed later
static
VOID LogBasicInfo (VOID) {
    UINTN  EfiMajorVersion = gST->Hdr.Revision >> 16;

#if REFIT_DEBUG > 0
    EFI_STATUS  Status;
    CHAR16     *MsgStr = NULL;
    UINT64      MaximumVariableSize;
    UINT64      MaximumVariableStorageSize;
    UINT64      RemainingVariableStorageSize;
    UINTN       HandleCount                        = 0;
    EFI_GUID    ConsoleControlProtocolGuid         = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;
    EFI_GUID    AppleFramebufferInfoProtocolGuid   = APPLE_FRAMEBUFFER_INFO_PROTOCOL_GUID;
    EFI_HANDLE *HandleBuffer                       = NULL;
    APPLE_FRAMEBUFFER_INFO_PROTOCOL *FramebufferInfo;

    LogRevisionInfo (&gST->Hdr, L"    System Table", sizeof(*gST), TRUE);
    LogRevisionInfo (&gBS->Hdr, L"   Boot Services", sizeof(*gBS), TRUE);
    LogRevisionInfo (&gRT->Hdr, L"Runtime Services", sizeof(*gRT), TRUE);
    LogRevisionInfo (&gDS->Hdr, L"    DXE Services", sizeof(*gDS), FALSE);
    MsgLog ("\n\n");
#endif

    WarnVersionEFI = WarnRevisionUEFI = FALSE;
    if (((gST->Hdr.Revision >> 16) != EfiMajorVersion)
        || ((gBS->Hdr.Revision >> 16) != EfiMajorVersion)
        || ((gRT->Hdr.Revision >> 16) != EfiMajorVersion)
    ) {
        WarnVersionEFI = TRUE;
    }
    else if (((gST->Hdr.Revision & 0xffff) != (gBS->Hdr.Revision & 0xffff))
        || ((gST->Hdr.Revision & 0xffff) != (gRT->Hdr.Revision & 0xffff))
        || ((gBS->Hdr.Revision & 0xffff) != (gRT->Hdr.Revision & 0xffff))
    ) {
        WarnRevisionUEFI = TRUE;
    }

#if REFIT_DEBUG > 0
    if (WarnVersionEFI || WarnRevisionUEFI) {
        if (WarnVersionEFI) {
            MsgLog ("** WARN: Inconsistent EFI Versions Detected");
            MsgLog ("%s         Program Behaviour is *NOT* Defined!!", OffsetNext);
        }
        else {
            MsgLog ("** WARN: Inconsistent UEFI Revisions Detected");
        }
        MsgLog ("\n\n");
    }

    /* NVRAM Storage Info */
    BOOLEAN QVInfoSupport = FALSE;
    MsgLog ("Non-Volatile Storage:");
#endif

    if (gRT->Hdr.Revision >> 16 > 1
        && MyStrStr (VendorInfo, L"Apple") == NULL
    ) {
        if ((gRT->Hdr.HeaderSize <
            MY_OFFSET_OF(EFI_RUNTIME_SERVICES, QueryVariableInfo) + sizeof(gRT->QueryVariableInfo))
        ) {
            WarnMissingQVInfo = TRUE;

#if REFIT_DEBUG > 0
            MsgLog ("\n\n");
            MsgLog ("** WARN: Inconsistent UEFI 2.x Implementation Detected");
            MsgLog ("%s         Program Behaviour is *NOT* Defined!!", OffsetNext);
        }
        else {
            Status = REFIT_CALL_4_WRAPPER(
                gRT->QueryVariableInfo,
                EFI_VARIABLE_NON_VOLATILE,
                &MaximumVariableStorageSize,
                &RemainingVariableStorageSize,
                &MaximumVariableSize
            );
            if (EFI_ERROR(Status)) {
                MsgLog ("\n\n");
                MsgLog ("** WARN: Could Not Retrieve Non-Volatile Storage Info");
            }
            else {
                MsgLog ("%s  - Total Storage         : %ld", OffsetNext, MaximumVariableStorageSize);
                MsgLog ("%s  - Remaining Available   : %ld", OffsetNext, RemainingVariableStorageSize);
                MsgLog ("%s  - Maximum Variable Size : %ld", OffsetNext, MaximumVariableSize);
            }
            QVInfoSupport = TRUE;
#endif
        }
    }
    // Ends here on RELEASE Builds

#if REFIT_DEBUG > 0
    if (!QVInfoSupport) {
        // QueryVariableInfo is not supported on Apple or EFI 1.x Firmware
        MsgLog ("%s*** QueryVariableInfo is Unavailable ***", OffsetNext);
    }
    MsgLog ("\n\n");

    /**
     * Report which video output devices are natively available. We do not actually
     * use them, so just use MsgStr as a throwaway pointer to the protocol.
    **/
    MsgLog ("ConsoleOut Modes:");

    Status = LibLocateProtocol (&ConsoleControlProtocolGuid, (VOID **) &MsgStr);
    MsgLog ("%s  - Text Mode             : %s", OffsetNext, EFI_ERROR(Status) ? L" NO" : L"YES");
    MY_FREE_POOL(MsgStr);

    Status = REFIT_CALL_3_WRAPPER(
        gBS->HandleProtocol,
        gST->ConsoleOutHandle,
        &gEfiUgaDrawProtocolGuid,
        (VOID **) &MsgStr
    );
    MsgLog ("%s  - Graphics Mode (UGA)   : %s", OffsetNext, EFI_ERROR(Status) ? L" NO" : L"YES");
    MY_FREE_POOL(MsgStr);

    Status = REFIT_CALL_3_WRAPPER(
        gBS->HandleProtocol,
        gST->ConsoleOutHandle,
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **) &MsgStr
    );
    MsgLog ("%s  - Graphics Mode (GOP)   : %s", OffsetNext, EFI_ERROR(Status) ? L" NO" : L"YES");
    MsgLog ("\n\n");
    MY_FREE_POOL(MsgStr);

    /* Secure Boot */
    MsgLog ("Shim:- '%s'", ShimLoaded()         ? L"Present" : L"Absent");
    MsgLog ("\n");
    MsgLog ("Secure Boot:- '%s'", secure_mode() ? L"Active"  : L"Inactive");
    MsgLog ("\n");

    /* Apple Framebuffers */
    Status = LibLocateProtocol (&AppleFramebufferInfoProtocolGuid, (VOID *) &FramebufferInfo);
    if (EFI_ERROR(Status)) {
        HandleCount = 0;
    }
    else {
        Status = gBS->LocateHandleBuffer (
            ByProtocol,
            &AppleFramebufferInfoProtocolGuid,
            NULL,
            &HandleCount,
            &HandleBuffer
        );
        if (EFI_ERROR(Status)) {
            HandleCount = 0;
        }
    }
    AppleFramebuffers = HandleCount;
    MsgLog ("Apple Framebuffers:- '%d'", AppleFramebuffers);
    MsgLog ("\n");
    MY_FREE_POOL(HandleBuffer);

    /* CSM Type */
    switch (GlobalConfig.LegacyType) {
        case LEGACY_TYPE_MAC:  MsgStr = StrDuplicate (L"Mac-Style");  break;
        case LEGACY_TYPE_UEFI: MsgStr = StrDuplicate (L"UEFI-Style"); break;
        case LEGACY_TYPE_NONE: MsgStr = StrDuplicate (L"Absent");     break;
        default:               MsgStr = StrDuplicate (L"Unknown");    break; // just in case
    }
    MsgLog ("Compat Support Module:- '%s'", MsgStr);
    MsgLog ("\n\n");
    MY_FREE_POOL(MsgStr);
#endif
} // VOID LogBasicInfo()

//
// main entry point
//
EFI_STATUS EFIAPI efi_main (
    EFI_HANDLE         ImageHandle,
    EFI_SYSTEM_TABLE  *SystemTable
) {
    EFI_STATUS  Status;

    BOOLEAN  MainLoopRunning = TRUE;
    BOOLEAN  MokProtocol     = FALSE;

    REFIT_MENU_ENTRY  *ChosenEntry    = NULL;
    LOADER_ENTRY      *ourLoaderEntry = NULL;
    LEGACY_ENTRY      *ourLegacyEntry = NULL;

    UINTN  i        = 0;
    UINTN  MenuExit = 0;

    EG_PIXEL  BGColor        = COLOR_LIGHTBLUE;
    CHAR16    *MsgStr        = NULL;
    CHAR16    *SelectionName = NULL;

    /* Enable Forced Native Logging */
    NativeLogger = TRUE;

    /* Bootstrap */
    InitializeLib (ImageHandle, SystemTable);
    Status = InitRefitLib (ImageHandle);

    if (EFI_ERROR(Status)) {
        return Status;
    }

    /* Init MainMenu */
    InitMainMenu();

    EFI_TIME Now;
    gRT->GetTime (&Now, NULL);
    NowYear   = Now.Year;
    NowMonth  = Now.Month;
    NowDay    = Now.Day;
    NowHour   = Now.Hour;
    NowMinute = Now.Minute;
    NowSecond = Now.Second;

    if (MyStrStr (gST->FirmwareVendor, L"Apple")) {
        VendorInfo = StrDuplicate (L"Apple");
    }
    else {
        VendorInfo = PoolPrint (
            L"%s %d.%02d",
            gST->FirmwareVendor,
            gST->FirmwareRevision >> 16,
            gST->FirmwareRevision & ((1 << 16) - 1)
        );
    }

    /* Init Logging */
    // DA-TAG: Also on RELEASE Builds as we need the timer
    InitBooterLog();

    #if REFIT_DEBUG > 0
    /* Start Logging */
    MsgLog (
        "Loading RefindPlus v%s on %s Firmware\n",
        REFINDPLUS_VERSION, VendorInfo
    );

    /* Architecture */
    MsgLog ("Arch/Type:- ");
#if defined(EFI32)
    MsgLog ("'x86 (32 bit)'");
#elif defined(EFIX64)
    MsgLog ("'x86 (64 bit)'");
#elif defined(EFIAARCH64)
    MsgLog ("'ARM (64 bit)'");
#else
    MsgLog ("'Not Known'");
#endif
    MsgLog ("\n");

    /* Build Engine */
    MsgLog ("Made With:- ");
#if defined(__MAKEWITH_GNUEFI)
    MsgLog ("'GNU-EFI'");
#else
    MsgLog ("'TianoCore EDK II'");
#endif
    MsgLog ("\n");

    /* TimeStamp */
    CHAR16 *CurDateStr = NULL;
    if (Now.TimeZone == EFI_UNSPECIFIED_TIMEZONE) {
        CurDateStr = PoolPrint (
            L"%d-%02d-%02d %02d:%02d:%02d (GMT)",
            NowYear, NowMonth,
            NowDay, NowHour,
            NowMinute, NowSecond
        );
    }
    else {
        CurDateStr = PoolPrint (
            L"%d-%02d-%02d %02d:%02d:%02d (GMT%s%02d:%02d)",
            NowYear, NowMonth,
            NowDay, NowHour,
            NowMinute, NowSecond,
            (Now.TimeZone > 0 ? L"-" : L"+"),
            ((ABS(Now.TimeZone)) / 60),
            ((ABS(Now.TimeZone)) % 60)
        );
    }
    MsgLog ("Timestamp:- '%s'", CurDateStr);
    MY_FREE_POOL(CurDateStr);
    #endif

    /* Set Legacy Boot Type */
    CopyMem (GlobalConfig.ScanFor, "ieom       ", NUM_SCAN_OPTIONS);
    FindLegacyBootType();
    if (GlobalConfig.LegacyType == LEGACY_TYPE_MAC) {
        CopyMem (GlobalConfig.ScanFor, "ihebocm    ", NUM_SCAN_OPTIONS);
    }

    /* Log System Details */
    LogBasicInfo ();

    /* Read Configuration */
    SetConfigFilename (ImageHandle);

    /* Set Secure Boot Up */
    MokProtocol = SecureBootSetup();

    #if REFIT_DEBUG > 0
    Status = (MokProtocol) ? EFI_SUCCESS : EFI_NOT_STARTED;
    MsgLog ("INFO: MOK Protocol ... %r", Status);
    MsgLog ("\n\n");
    #endif

    // First scan volumes by calling ScanVolumes() to find "SelfVolume",
    //    SelfVolume is required by LoadDrivers() and ReadConfig();
    // A second call is needed later to enumerate volumes as well as
    //    to register new filesystem(s) accessed by drivers.
    ScanVolumes();

    /* Get/Set Config File ... Prefer RefindPlus Configuration File Naame */
    if (!FileExists (SelfDir, GlobalConfig.ConfigFilename)) {
        ConfigWarn = TRUE;

        #if REFIT_DEBUG > 0
        MsgLog ("** WARN: Could not find RefindPlus configuration file:- 'config.conf'\n");
        MsgLog ("         Trying rEFInd's configuration file instead:- 'refind.conf'\n");
        MsgLog ("         Provide a 'config.conf' file to silence this warning\n");
        MsgLog ("         You can rename 'refind.conf' as 'config.conf'\n");
        MsgLog ("         NB: Will not contain all RefindPlus settings\n\n");
        #endif

        GlobalConfig.ConfigFilename = L"refind.conf";
    }

    /* Read config file to get tokens that may be required by 'LoadDrivers' */
    ReadConfig (GlobalConfig.ConfigFilename);
    AdjustDefaultSelection();


    #if REFIT_DEBUG > 0
    MsgLog ("INFO: RefitDBG:- '%d'",                 REFIT_DEBUG                                            );
    MsgLog ("%s      LogLevel:- '%d'",   OffsetNext, GlobalConfig.LogLevel                                  );
    MsgLog ("%s      ScanDelay:- '%d'",  OffsetNext, GlobalConfig.ScanDelay                                 );
    MsgLog ("%s      ReloadGOP:- %s",    OffsetNext, GlobalConfig.ReloadGOP    ? L"'YES'"    : L"'NO'"      );
    MsgLog ("%s      SyncAPFS:- %s",     OffsetNext, GlobalConfig.SyncAPFS     ? L"'Active'" : L"'Inactive'");
    MsgLog ("%s      TagsHelp:- %s",     OffsetNext, GlobalConfig.TagsHelp     ? L"'Active'" : L"'Inactive'");
    MsgLog ("%s      TextOnly:- %s",     OffsetNext, GlobalConfig.TextOnly     ? L"'Active'" : L"'Inactive'");
    MsgLog ("%s      ScanAllESP:- %s",   OffsetNext, GlobalConfig.ScanAllESP   ? L"'Active'" : L"'Inactive'");
    MsgLog ("%s      TextRenderer:- %s", OffsetNext, GlobalConfig.TextRenderer ? L"'Active'" : L"'Inactive'");

    MsgLog ("%s      ProtectNVRAM:- ",   OffsetNext                          );
    if (MyStrStr (VendorInfo, L"Apple") == NULL) {
        MsgLog ("'Disabled'"                                                 );
    }
    else {
        MsgLog ("%s", GlobalConfig.ProtectNVRAM ? L"'Active'" : L"'Inactive'");
    }

    MsgLog (
        "%s      NormaliseCSR:- %s",
        OffsetNext,
        GlobalConfig.NormaliseCSR ? L"'Active'" : L"'Inactive'"
    );
    MsgLog (
        "%s      IgnorePreviousBoot:- %s",
        OffsetNext,
        GlobalConfig.IgnorePreviousBoot ? L"'Active'" : L"'Inactive'"
    );

    // DA-TAG: Prime Status for SupplyAPFS
    //         Here to accomodate GNU-RFI
    Status = EFI_NOT_STARTED;
    #endif

    #ifdef __MAKEWITH_TIANO
    // DA-TAG: Limit to TianoCore
    if (GlobalConfig.SupplyAPFS) {
        Status = RP_ApfsConnectDevices();
    }
    #endif

    #if REFIT_DEBUG > 0
    MsgLog ("\n\n");
    MsgLog ("INFO: Supply APFS Support ... %r", Status);

    // DA-TAG: Prime Status for SupplNVME
    //         Here to accomodate GNU-RFI
    Status = EFI_NOT_STARTED;
    #endif

    #ifdef __MAKEWITH_TIANO
    // DA-TAG: Limit to TianoCore
    if (GlobalConfig.SupplyNVME) {
        Status = NvmExpressLoad (ImageHandle, SystemTable);
    }
    #endif

    #if REFIT_DEBUG > 0
    MsgLog ("%s      Supply NVMe Support ... %r", OffsetNext, Status);
    MsgLog ("\n\n");
    #endif

    // Load Drivers
    LoadDrivers();

    // Restore SystemTable if previously amended
    if (SetSysTab) {
        // Reinitialise
        InitializeLib (ImageHandle, SystemTable);

        #if REFIT_DEBUG > 0
        Status = EFI_SUCCESS;
        MsgStr = PoolPrint (L"Restore System Table ... %r", Status);
        LOG(1, LOG_STAR_SEPARATOR, L"%s", MsgStr);
        MsgLog ("%s      %s", OffsetNext, MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif
    }

    // Second call to ScanVolumes() to enumerate volumes and
    //   register any new filesystem(s) accessed by drivers.
    ScanVolumes();

    if (GlobalConfig.SpoofOSXVersion &&
        GlobalConfig.SpoofOSXVersion[0] != L'\0'
    ) {
        Status = SetAppleOSInfo();

        #if REFIT_DEBUG > 0
        MsgLog ("INFO: Spoof Mac OS Version ... %r\n\n", Status);
        #endif
    }

    #if REFIT_DEBUG > 0
    MsgStr = StrDuplicate (L"Initialise Screen");
    LOG(1, LOG_LINE_SEPARATOR, L"%s", MsgStr);
    MsgLog ("%s...", MsgStr);
    MsgLog ("\n");
    MY_FREE_POOL(MsgStr);
    #endif

    InitScreen();

    WarnIfLegacyProblems();
    MainMenu->TimeoutSeconds = GlobalConfig.Timeout;

    // disable EFI watchdog timer
    REFIT_CALL_4_WRAPPER(
        gBS->SetWatchdogTimer,
        0x0000, 0x0000, 0x0000,
        NULL
    );

    // Further bootstrap (now with config available)
    SetupScreen();

    /* Disable Forced Native Logging */
    NativeLogger = FALSE;

    // Show Secure Boot Failure Notice and Shut Down
    if (SecureBootFailure) {
        #if REFIT_DEBUG > 0
        MsgStr = StrDuplicate (L"Secure Boot Failure");
        LOG(1, LOG_LINE_SEPARATOR, L"Display %s Warning", MsgStr);
        MsgLog ("INFO: User Warning:- '%s ... Forcing Shutdown'\n\n", MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif

        SwitchToText (FALSE);

        MuteLogger = TRUE;
        REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_ERROR);
        PrintUglyText (L"                                                          ", NEXTLINE);
        PrintUglyText (L"              Failure Setting Secure Boot Up              ", NEXTLINE);
        PrintUglyText (L"              Forcing Shutdown in 9 Seconds!              ", NEXTLINE);
        PrintUglyText (L"                                                          ", NEXTLINE);
        REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_BASIC);
        MuteLogger = FALSE;

        PauseSeconds (9);

        REFIT_CALL_4_WRAPPER(
            gRT->ResetSystem,
            EfiResetShutdown,
            EFI_SUCCESS,
            0, NULL
        );
    }

    // Apply Scan Delay if set
    if (GlobalConfig.ScanDelay > 0) {
        MsgStr = StrDuplicate (L"Paused for Scan Delay");

        UINTN Trigger = 3;
        if (GlobalConfig.ScanDelay > Trigger) {
            CHAR16 *PartMsg = PoolPrint (L"%s ... Please Wait", MsgStr);
            egDisplayMessage (PartMsg, &BGColor, CENTER);

            MY_FREE_POOL(PartMsg);
        }

        #if REFIT_DEBUG > 0
        LOG(1, LOG_LINE_THIN_SEP, L"Scan Delay");
        LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
        MsgLog ("%s:\n", MsgStr);
        #endif

        MY_FREE_POOL(MsgStr);

        for (i = 0; i < GlobalConfig.ScanDelay; ++i) {
            #if REFIT_DEBUG > 0
            LOG(1, LOG_THREE_STAR_MID, L"Loading Paused for 1 Second");
            #endif

            // Wait 1 second
            REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
            REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
            REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
            REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
        }

        #if REFIT_DEBUG > 0
        MsgStr = PoolPrint (L"Resuming After a %d Second Pause", i);
        LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
        MsgLog ("  - %s\n", MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif

        if (GlobalConfig.ScanDelay > Trigger) {
            BltClearScreen (TRUE);
        }
    }

    // Continue Bootstrap
    SetVolumeIcons();
    ScanForBootloaders (FALSE);
    ScanForTools();

    if (GlobalConfig.ShutdownAfterTimeout) {
        MainMenu->TimeoutText = StrDuplicate (L"Shutdown");
    }

    // Show EFI Version Mismatch Warning
    if (WarnVersionEFI) {
        #if REFIT_DEBUG > 0
        MsgStr = StrDuplicate (L"Inconsistent EFI Versions");
        LOG(1, LOG_LINE_SEPARATOR, L"Display %s Warning", MsgStr);
        MsgLog ("INFO: User Warning:- '%s'\n\n", MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif

        SwitchToText (FALSE);

        MuteLogger = TRUE;
        REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_ERROR);
        PrintUglyText (L"                                                          ", NEXTLINE);
        PrintUglyText (L"            Inconsistent EFI Versions Detected            ", NEXTLINE);
        PrintUglyText (L"           Program Behaviour is *NOT* Defined!!           ", NEXTLINE);
        PrintUglyText (L"                                                          ", NEXTLINE);
        REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_BASIC);
        MuteLogger = FALSE;

        PauseForKey();

        #if REFIT_DEBUG > 0
        MsgStr = StrDuplicate (L"Warning Acknowledged or Timed Out");
        LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
        LOG(1, LOG_BLANK_LINE_SEP, L"X");
        MsgLog ("INFO: %s ... ", MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif
        if (AllowGraphicsMode) {
            #if REFIT_DEBUG > 0
            MsgLog ("Restore Graphics Mode\n\n");
            #endif

            SwitchToGraphicsAndClear (TRUE);
        }
        else {
            #if REFIT_DEBUG > 0
            MsgLog ("Proceeding\n\n");
            #endif
        }
        // Wait 1 second
        REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
        REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
        REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
        REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
    }

    // Show Inconsistent UEFI 2.x Implementation Warning
    if (WarnMissingQVInfo) {
        #if REFIT_DEBUG > 0
        MsgStr = StrDuplicate (L"Inconsistent UEFI 2.x Implementation");
        LOG(1, LOG_LINE_SEPARATOR, L"Display %s Warning", MsgStr);
        MsgLog ("INFO: User Warning:- '%s'\n\n", MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif

        SwitchToText (FALSE);

        MuteLogger = TRUE;
        REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_ERROR);
        PrintUglyText (L"                                                          ", NEXTLINE);
        PrintUglyText (L"       Inconsistent UEFI 2.x Implementation Detected      ", NEXTLINE);
        PrintUglyText (L"           Program Behaviour is *NOT* Defined!!           ", NEXTLINE);
        PrintUglyText (L"                                                          ", NEXTLINE);
        REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_BASIC);
        MuteLogger = FALSE;

        PauseForKey();

        #if REFIT_DEBUG > 0
        MsgStr = StrDuplicate (L"Warning Acknowledged or Timed Out");
        LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
        LOG(1, LOG_BLANK_LINE_SEP, L"X");
        MsgLog ("      %s ...", MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif

        if (AllowGraphicsMode) {
            #if REFIT_DEBUG > 0
            MsgLog ("Restore Graphics Mode");
            #endif

            SwitchToGraphicsAndClear (TRUE);
        }
        else {
            #if REFIT_DEBUG > 0
            MsgLog ("Proceeding");
            #endif

            BltClearScreen (FALSE);
        }

        #if REFIT_DEBUG > 0
        MsgLog ("\n\n");
        #endif

        // Wait 1 second
        REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
        REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
        REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
        REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
    }

    // Show Config Mismatch Warning
    if (ConfigWarn) {
        #if REFIT_DEBUG > 0
        MsgStr = StrDuplicate (L"Missing Config File");
        LOG(1, LOG_LINE_SEPARATOR, L"Display %s Warning", MsgStr);
        MsgLog ("INFO: User Warning:- '%s'\n\n", MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif

        SwitchToText (FALSE);

        MuteLogger = TRUE;
        REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_ERROR);
        PrintUglyText (L"                                                          ", NEXTLINE);
        PrintUglyText (L"      Could not find a RefindPlus 'config.conf' file      ", NEXTLINE);
        PrintUglyText (L"    Trying to load a rEFInd 'refind.conf' file instead    ", NEXTLINE);
        PrintUglyText (L"                                                          ", NEXTLINE);

        REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_BASIC);
        PrintUglyText (L"                                                          ", NEXTLINE);
        PrintUglyText (L"    Provide a config.conf file to silence this warning    ", NEXTLINE);
        PrintUglyText (L"     You can rename a refind.conf file as config.conf     ", NEXTLINE);
        PrintUglyText (L" * Will not contain all RefindPlus configuration tokens * ", NEXTLINE);
        PrintUglyText (L"                                                          ", NEXTLINE);
        MuteLogger = FALSE;

        PauseForKey();

        #if REFIT_DEBUG > 0
        MsgStr = StrDuplicate (L"Warning Acknowledged or Timed Out");
        LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
        LOG(1, LOG_BLANK_LINE_SEP, L"X");
        MsgLog ("      %s ...", MsgStr);
        MY_FREE_POOL(MsgStr);
        #endif

        if (AllowGraphicsMode) {
            #if REFIT_DEBUG > 0
            MsgLog ("Restore Graphics Mode\n\n");
            #endif

            SwitchToGraphicsAndClear (TRUE);
        }
        else {
            #if REFIT_DEBUG > 0
            MsgLog ("Proceeding\n\n");
            #endif
        }
        // Wait 0.25 second
        REFIT_CALL_1_WRAPPER(gBS->Stall, 250000);
    }

    if (GlobalConfig.LogLevel > 1) {
        /* Enable Forced Native Logging */
        NativeLogger = TRUE;
    }

    pdInitialize();
    /* Disable Forced Native Logging */
    NativeLogger = FALSE;

    #if REFIT_DEBUG > 0
    LOG(1, LOG_LINE_SEPARATOR, L"Entering Main Loop");
    #endif

    if (GlobalConfig.DefaultSelection) {
        SelectionName = StrDuplicate (GlobalConfig.DefaultSelection);
    }

    BOOLEAN ProtectNVRAM = FALSE;
    while (MainLoopRunning) {
        // Set to false as may not be booting
        IsBoot = FALSE;

        // Reset NVRAM Protection
        if (ProtectNVRAM) {
            ProtectNVRAM                               = FALSE;
            RT->SetVariable                            = gRT->SetVariable;
            gRT->SetVariable                           = gRT->SetVariable;
            SystemTable->RuntimeServices->SetVariable  = gRT->SetVariable;
        }

        MenuExit = RunMainMenu (MainMenu, &SelectionName, &ChosenEntry);

        // Ignore MenuExit if FlushFailedTag is set and not previously reset
        if (FlushFailedTag && !FlushFailReset) {
            #if REFIT_DEBUG > 0
            MsgStr = StrDuplicate (L"FlushFailedTag is Set ... Ignore MenuExit");
            LOG(1, LOG_STAR_SEPARATOR, L"%s", MsgStr);
            MsgLog ("INFO: %s\n\n", MsgStr);
            MY_FREE_POOL(MsgStr);
            #endif

            FlushFailedTag = FALSE;
            FlushFailReset = TRUE;
            continue;
        }

        // The Escape key triggers a re-scan operation
        if (MenuExit == MENU_EXIT_ESCAPE) {
            #if REFIT_DEBUG > 0
            MsgLog ("User Input Received:");
            MsgLog ("%s  - Escape Key Pressed ... Rescan All\n\n", OffsetNext);
            #endif

            RescanAll (TRUE, TRUE);
            continue;
        }

        if ((MenuExit == MENU_EXIT_TIMEOUT) &&
            GlobalConfig.ShutdownAfterTimeout
        ) {
            ChosenEntry->Tag = TAG_SHUTDOWN;
        }

        switch (ChosenEntry->Tag) {
            case TAG_INFO_NVRAMCLEAN:    // Clean NVRAM Info
                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_THIN_SEP, L"Creating 'Clean NVRAM Info' Screen");

                MsgLog ("User Input Received:");
                MsgLog ("%s  - Show Clean NVRAM Info\n\n", OffsetNext);
                #endif

                preCleanNvram();

                // Reboot if CleanNvram was triggered
                if (ranCleanNvram) {
                    #if REFIT_DEBUG > 0
                    LOG(1, LOG_LINE_NORMAL, L"Cleaned NVRAM");

                    MsgLog ("INFO: Cleaned Nvram\n\n");
                    MsgLog ("Terminating Screen:\n");
                    MsgLog ("System Restart...\n");
                    #endif

                    TerminateScreen();

                    #if REFIT_DEBUG > 0
                    LOG(1, LOG_LINE_NORMAL, L"System Restart");

                    MsgLog ("System Restarting");
                    MsgLog ("\n-----------------\n\n");
                    #endif

                    REFIT_CALL_4_WRAPPER(
                        gRT->ResetSystem,
                        EfiResetCold,
                        EFI_SUCCESS,
                        0, NULL
                    );

                    MsgStr = StrDuplicate (L"System Restart FAILED!!");
                    #if REFIT_DEBUG > 0
                    LOG(1, LOG_THREE_STAR_SEP, MsgStr);
                    MsgLog ("INFO: %s\n\n", MsgStr);
                    #endif

                    MuteLogger = TRUE;
                    REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_ERROR);
                    PrintUglyText (MsgStr, NEXTLINE);
                    REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_BASIC);
                    MuteLogger = FALSE;

                    MY_FREE_POOL(MsgStr);

                    PauseForKey();

                    // just in case we get this far
                    MainLoopRunning = FALSE;
                }
                break;

            case TAG_LOAD_NVRAMCLEAN:    // Clean NVRAM
                ourLoaderEntry = (LOADER_ENTRY *) ChosenEntry;

                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"Cleaning NVRAM");

                MsgLog ("User Input Received:");
                MsgLog ("%s  - Clean NVRAM", OffsetNext);
                if (egIsGraphicsModeEnabled()) {
                    MsgLog ("\n-----------------\n\n");
                }
                else {
                    MsgLog ("\n\n");
                }
                #endif

                StartTool (ourLoaderEntry);
                break;

            case TAG_INFO_BOOTKICKER:    // Apple Boot Screen Info
                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_THIN_SEP, L"Creating 'BootKicker Info' Screen");

                MsgLog ("User Input Received:");
                MsgLog ("%s  - Show BootKicker Info\n\n", OffsetNext);
                #endif

                preBootKicker();
                break;

            case TAG_LOAD_BOOTKICKER:    // Apple Boot Screen
                ourLoaderEntry = (LOADER_ENTRY *) ChosenEntry;

                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"Loading Apple Boot Screen");

                MsgLog ("User Input Received:");
                MsgLog ("%s  - Load Apple Boot Screen", OffsetNext);
                if (egIsGraphicsModeEnabled()) {
                    MsgLog ("\n-----------------\n\n");
                }
                else {
                    MsgLog ("\n\n");
                }
                #endif

                ourLoaderEntry->UseGraphicsMode = TRUE;

                StartTool (ourLoaderEntry);
                break;

            case TAG_REBOOT:    // Reboot
                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - System Restart", OffsetNext);
                if (egIsGraphicsModeEnabled()) {
                    MsgLog ("\n-----------------\n\n");
                }
                else {
                    MsgLog ("\n\n");
                }
                #endif

                TerminateScreen();

                #if REFIT_DEBUG > 0
                LOG(1, LOG_STAR_SEPARATOR, L"Restarting System");
                #endif

                REFIT_CALL_4_WRAPPER(
                    gRT->ResetSystem,
                    EfiResetCold,
                    EFI_SUCCESS,
                    0, NULL
                );

                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"Restart FAILED!!");
                #endif

                // just in case we get this far
                MainLoopRunning = FALSE;
                break;

            case TAG_SHUTDOWN: // Shut Down
                #if REFIT_DEBUG > 0
                LOG(1, LOG_STAR_SEPARATOR, L"Shutting System Down");

                MsgLog ("User Input Received:");
                MsgLog ("%s  - System Shutdown", OffsetNext);
                if (egIsGraphicsModeEnabled()) {
                    MsgLog ("\n-----------------\n\n");
                }
                else {
                    MsgLog ("\n\n");
                }
                #endif

                TerminateScreen();

                REFIT_CALL_4_WRAPPER(
                    gRT->ResetSystem,
                    EfiResetShutdown,
                    EFI_SUCCESS,
                    0, NULL
                );

                #if REFIT_DEBUG > 0
                LOG(1, LOG_LINE_NORMAL, L"Shutdown FAILED!!");
                #endif

                // just in case we get this far
                MainLoopRunning = FALSE;
                break;

            case TAG_ABOUT:    // About RefindPlus
                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Show 'About RefindPlus' Page\n\n", OffsetNext);
                #endif

                AboutRefindPlus();

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Exit 'About RefindPlus' Page\n\n", OffsetNext);
                #endif

                break;

            case TAG_LOADER:   // Boot OS via *.efi File
                ourLoaderEntry = (LOADER_ENTRY *) ChosenEntry;

                // Fix undetected Mac OS
                if (!FoundSubStr (ourLoaderEntry->Title, L"Mac OS") &&
                    FoundSubStr (ourLoaderEntry->LoaderPath, L"System\\Library\\CoreServices")
                ) {
                    if (MyStriCmp (ourLoaderEntry->Volume->VolName, L"PreBoot")) {
                        ourLoaderEntry->Title = L"Mac OS";
                    }
                    else {
                        ourLoaderEntry->Title = L"RefindPlus";
                    }
                }

                // Fix undetected Windows
                if (!FoundSubStr (ourLoaderEntry->Title, L"Windows") &&
                    FoundSubStr (ourLoaderEntry->LoaderPath, L"EFI\\Microsoft\\Boot")
                ) {
                    ourLoaderEntry->Title = L"Windows (UEFI)";
                }

                if (FoundSubStr (ourLoaderEntry->Title, L"OpenCore") ||
                    FoundSubStr (ourLoaderEntry->LoaderPath, L"\\OpenCore")
                ) {
                    // Set CSR if required
                    ActiveCSR();

                    if (!ourLoaderEntry->UseGraphicsMode) {
                        ourLoaderEntry->UseGraphicsMode = GlobalConfig.GraphicsFor & GRAPHICS_FOR_OPENCORE;
                    }

                    #if REFIT_DEBUG > 0
                    // DA-TAG: Using separate instances of 'User Input Received:'
                    MsgLog ("User Input Received:");
                    MsgStr = StrDuplicate (L"Load OpenCore Instance");
                    LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                    MsgLog (
                        "%s  - %s:- '%s'",
                        OffsetNext,
                        MsgStr,
                        ourLoaderEntry->LoaderPath
                    );
                    MY_FREE_POOL(MsgStr);
                    #endif

                    // Filter out the 'APPLE_INTERNAL' CSR bit if required
                    FilterCSR();
                }
                else if (FoundSubStr (ourLoaderEntry->Title, L"Clover") ||
                    FoundSubStr (ourLoaderEntry->LoaderPath, L"\\Clover")
                ) {
                    // Set CSR if required
                    ActiveCSR();

                    if (!ourLoaderEntry->UseGraphicsMode) {
                        ourLoaderEntry->UseGraphicsMode = GlobalConfig.GraphicsFor & GRAPHICS_FOR_CLOVER;
                    }

                    #if REFIT_DEBUG > 0
                    // DA-TAG: Using separate instances of 'User Input Received:'
                    MsgLog ("User Input Received:");
                    MsgStr = StrDuplicate (L"Load Clover Instance");
                    LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                    MsgLog (
                        "%s  - %s:- '%s'",
                        OffsetNext,
                        MsgStr,
                        ourLoaderEntry->LoaderPath
                    );
                    MY_FREE_POOL(MsgStr);
                    #endif

                    // Filter out the 'APPLE_INTERNAL' CSR bit if required
                    FilterCSR();
                }
                else if (FoundSubStr (ourLoaderEntry->Title, L"Mac OS")) {
                    // Set CSR if required
                    ActiveCSR();

                    #if REFIT_DEBUG > 0
                    // DA-TAG: Using separate instances of 'User Input Received:'
                    MsgLog ("User Input Received:");
                    MsgStr = StrDuplicate (L"Boot Mac OS");
                    LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                    MsgLog ("%s  - %s", OffsetNext, MsgStr);

                    CHAR16 *DisplayName = NULL;
                    if (!ourLoaderEntry->Volume->VolName) {
                        MsgLog (":- '%s'", ourLoaderEntry->LoaderPath);
                    }
                    else {
                        if (GlobalConfig.SyncAPFS && ourLoaderEntry->Volume->FSType == FS_TYPE_APFS) {
                            APPLE_APFS_VOLUME_ROLE VolumeRole = 0;

                            // DA-TAG: Limit to TianoCore
                            #ifdef __MAKEWITH_GNUEFI
                            Status = EFI_NOT_STARTED;
                            #else
                            Status = RP_GetApfsVolumeInfo (
                                ourLoaderEntry->Volume->DeviceHandle,
                                NULL, NULL,
                                &VolumeRole
                            );
                            #endif

                            if (!EFI_ERROR(Status)) {
                                if (VolumeRole == APPLE_APFS_VOLUME_ROLE_PREBOOT) {
                                    DisplayName = GetVolumeGroupName (
                                        ourLoaderEntry->LoaderPath,
                                        ourLoaderEntry->Volume
                                    );
                                }
                            }
                        } // if GlobalConfig.SyncAFPS

                        MsgLog (
                            " from '%s'",
                            DisplayName
                                ? DisplayName
                                : ourLoaderEntry->Volume->VolName
                        );
                    }
                    MY_FREE_POOL(MsgStr);
                    MY_FREE_POOL(DisplayName);
                    #endif

                    // Enable TRIM on non-Apple SSDs if configured to
                    if (GlobalConfig.ForceTRIM) {
                        ForceTRIM();
                    }

                    // Set Mac boot args if configured to
                    // Also disables AMFI if this is configured
                    // Also disables Mac OS compatibility check if this is configured
                    if (GlobalConfig.SetBootArgs && GlobalConfig.SetBootArgs[0] != L'\0') {
                        SetBootArgs();
                    }
                    else {
                        if (GlobalConfig.DisableAMFI) {
                            // Disable AMFI if configured to
                            // Also disables Mac OS compatibility check if this is configured
                            DisableAMFI();
                        }
                        else if (GlobalConfig.DisableCompatCheck) {
                            // Disable Mac OS compatibility check if configured to
                            DisableCompatCheck();
                        }
                    }

                    // Filter out the 'APPLE_INTERNAL' CSR bit if required
                    FilterCSR();

                    // Re-Map OpenProtocol
                    ReMapOpenProtocol();
                }
                else if (FoundSubStr (ourLoaderEntry->Title, L"Windows")) {
                    if (GlobalConfig.ProtectNVRAM) {
                        // Protect Mac NVRAM from UEFI Windows
                        ProtectNVRAM                               = TRUE;
                        AltSetVariable                             = gRT->SetVariable;
                        RT->SetVariable                            = gRTSetVariableEx;
                        gRT->SetVariable                           = gRTSetVariableEx;
                        SystemTable->RuntimeServices->SetVariable  = gRTSetVariableEx;
                    }

                    #if REFIT_DEBUG > 0
                    // DA-TAG: Using separate instances of 'User Input Received:'
                    MsgLog ("User Input Received:");
                    MsgStr = StrDuplicate (L"Boot UEFI Windows");
                    LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                    MsgLog ("%s  - %s", OffsetNext, MsgStr);
                    if (ourLoaderEntry->Volume->VolName) {
                        MsgLog (" from '%s'", ourLoaderEntry->Volume->VolName);
                    }
                    else {
                        MsgLog (":- '%s'", ourLoaderEntry->LoaderPath);
                    }

                    MY_FREE_POOL(MsgStr);
                    #endif
                }
                else if (FoundSubStr (ourLoaderEntry->Title, L"Linux")) {
                    #if REFIT_DEBUG > 0
                    // DA-TAG: Using separate instances of 'User Input Received:'
                    MsgLog ("User Input Received:");
                    MsgStr = StrDuplicate (L"Boot Linux");
                    LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                    MsgLog ("%s  - %s", OffsetNext, MsgStr);
                    if (ourLoaderEntry->Volume->VolName) {
                        MsgLog (" from '%s'", ourLoaderEntry->Volume->VolName);
                    }
                    else {
                        MsgLog (":- '%s'", ourLoaderEntry->LoaderPath);
                    }

                    MY_FREE_POOL(MsgStr);
                    #endif
                }
                else {
                    #if REFIT_DEBUG > 0
                    MsgStr = StrDuplicate (L"Boot Child Image via UEFI Loader");
                    LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                    // DA-TAG: Using separate instances of 'User Input Received:'
                    MsgLog ("User Input Received:");
                    MsgLog (
                        "%s  - %s:- '%s'",
                        OffsetNext, MsgStr,
                        ourLoaderEntry->LoaderPath
                    );

                    MY_FREE_POOL(MsgStr);
                    #endif
                }

                #if REFIT_DEBUG > 0
                MsgLog ("\n-----------------\n\n");
                #endif

                StartLoader (ourLoaderEntry, SelectionName);
                break;

            case TAG_LEGACY:   // Boot legacy OS
                ourLegacyEntry = (LEGACY_ENTRY *) ChosenEntry;

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                #endif

                if (MyStrStr (ourLegacyEntry->Volume->OSName, L"Windows")) {
                    #if REFIT_DEBUG > 0
                    MsgStr = PoolPrint (
                        L"Boot %s from '%s'",
                        ourLegacyEntry->Volume->OSName,
                        ourLegacyEntry->Volume->VolName
                    );
                    LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                    MsgLog ("%s  - %s", OffsetNext, MsgStr);
                    MY_FREE_POOL(MsgStr);
                    #endif
                }
                else {
                    #if REFIT_DEBUG > 0
                    MsgStr = StrDuplicate (L"Boot 'Mac-Style' Legacy (BIOS) OS");
                    LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                    MsgLog ("%s  - %s:- '%s'", OffsetNext, MsgStr, ourLegacyEntry->Volume->OSName);
                    MY_FREE_POOL(MsgStr);
                    #endif
                }

                #if REFIT_DEBUG > 0
                if (egIsGraphicsModeEnabled()) {
                    MsgLog ("\n-----------------\n\n");
                }
                else {
                    MsgLog ("\n\n");
                }
                #endif

                StartLegacy (ourLegacyEntry, SelectionName);
                break;

            case TAG_LEGACY_UEFI: // Boot a legacy OS on a non-Mac
                ourLegacyEntry = (LEGACY_ENTRY *) ChosenEntry;

                #if REFIT_DEBUG > 0
                MsgStr = StrDuplicate (L"Boot 'UEFI-Style' Legacy (BIOS) OS");
                LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                MsgLog ("User Input Received:");
                MsgLog (
                    "%s  - %s:- '%s'",
                    OffsetNext, MsgStr,
                    ourLegacyEntry->Volume ? ourLegacyEntry->Volume->OSName : L"NULL Volume"
                );
                MY_FREE_POOL(MsgStr);

                if (egIsGraphicsModeEnabled()) {
                    MsgLog ("\n-----------------\n\n");
                }
                else {
                    MsgLog ("\n\n");
                }
                #endif

                StartLegacyUEFI (ourLegacyEntry, SelectionName);
                break;

            case TAG_TOOL:     // Start a UEFI tool
                ourLoaderEntry = (LOADER_ENTRY *) ChosenEntry;

                #if REFIT_DEBUG > 0
                MsgStr = StrDuplicate (L"Start UEFI Tool");
                LOG(1, LOG_LINE_THIN_SEP, L"%s", MsgStr);
                MsgLog ("User Input Received:");
                MsgLog ("%s  - %s:- '%s'", OffsetNext, MsgStr, ourLoaderEntry->LoaderPath);
                MsgLog ("\n\n");
                MY_FREE_POOL(MsgStr);
                #endif

                if (FoundSubStr (ourLoaderEntry->Title, L"Boot Screen")) {
                    ourLoaderEntry->UseGraphicsMode = TRUE;
                }

                StartTool (ourLoaderEntry);
                break;

            case TAG_FIRMWARE_LOADER:  // Reboot to a loader defined in the EFI UseNVRAM
                ourLoaderEntry = (LOADER_ENTRY *) ChosenEntry;

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Reboot into Firmaware Defined Loader", OffsetNext);
                MsgLog ("\n-----------------\n\n");
                #endif

                RebootIntoLoader (ourLoaderEntry);
                break;

            case TAG_HIDDEN:  // Manage hidden tag entries

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Manage Hidden Tag Entries\n\n", OffsetNext);
                #endif

                ManageHiddenTags();

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Exit Hidden Tags Page\n\n", OffsetNext);
                #endif

                break;

            case TAG_EXIT:    // Exit RefindPlus

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Exit RefindPlus", OffsetNext);
                if (egIsGraphicsModeEnabled()) {
                    MsgLog ("\n-----------------\n\n");
                }
                else {
                    MsgLog ("\n\n");
                }
                #endif

                if ((MokProtocol) && !SecureBootUninstall()) {
                    // just in case we get this far
                    MainLoopRunning = FALSE;
                }
                else {
                   BeginTextScreen (L" ");
                   return EFI_SUCCESS;
                }
                break;

            case TAG_FIRMWARE: // Reboot into firmware's user interface

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Reboot into Firmware", OffsetNext);
                if (egIsGraphicsModeEnabled()) {
                    MsgLog ("\n-----------------\n\n");
                }
                else {
                    MsgLog ("\n\n");
                }
                #endif

                RebootIntoFirmware();
                break;

            case TAG_CSR_ROTATE:

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Toggle Mac CSR\n", OffsetNext);
                #endif

                RotateCsrValue();
                break;

            case TAG_INSTALL:

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Install RefindPlus", OffsetNext);
                if (egIsGraphicsModeEnabled()) {
                    MsgLog ("\n-----------------\n\n");
                }
                else {
                    MsgLog ("\n\n");
                }
                #endif

                InstallRefindPlus();
                break;

            case TAG_BOOTORDER:

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Manage Firmware Boot Order\n\n", OffsetNext);
                #endif

                ManageBootorder();

                #if REFIT_DEBUG > 0
                MsgLog ("User Input Received:");
                MsgLog ("%s  - Exit Manage Firmware Boot Order Page\n\n", OffsetNext);
                #endif

                break;
        } // switch
    } // while

    // If we end up here, things have gone wrong. Try to reboot, and if that
    // fails, go into an endless loop.
    #if REFIT_DEBUG > 0
    LOG(1, LOG_THREE_STAR_SEP, L"Unexpected Main Loop Exit ... Try to Reboot!!");

    MsgLog ("Fallback: System Restart...\n");
    MsgLog ("Screen Termination:\n");
    #endif

    TerminateScreen();

    #if REFIT_DEBUG > 0
    MsgLog ("System Reset:\n\n");
    #endif

    REFIT_CALL_4_WRAPPER(
        gRT->ResetSystem,
        EfiResetCold,
        EFI_SUCCESS,
        0, NULL
    );

    #if REFIT_DEBUG > 0
    LOG(1, LOG_THREE_STAR_SEP, L"Shutdown After Main Loop Exit:- 'FAILED'!!");
    #endif

    SwitchToText (FALSE);

    MsgStr = StrDuplicate (L"Reboot Failed ... Entering Endless Idle Loop");

    MuteLogger = TRUE;
    REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_ERROR);
    PrintUglyText (MsgStr, NEXTLINE);
    REFIT_CALL_2_WRAPPER(gST->ConOut->SetAttribute, gST->ConOut, ATTR_BASIC);
    MuteLogger = FALSE;

    #if REFIT_DEBUG > 0
    LOG(1, LOG_BLANK_LINE_SEP, L"X");
    LOG(1, LOG_LINE_NORMAL, L"%s", MsgStr);
    MsgLog ("INFO: %s", MsgStr);
    MsgLog ("\n-----------------\n\n");
    #endif

    MY_FREE_POOL(MsgStr);

    PauseForKey();
    EndlessIdleLoop();

    return EFI_SUCCESS;
} // EFI_STATUS EFIAPI efi_main()
