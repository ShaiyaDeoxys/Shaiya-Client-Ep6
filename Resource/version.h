#pragma once

// ---- Srole client side — Versiyon Bilgisi ----
// Bu dosyadaki değerleri değiştirerek RC/manifest içeriğini otomatik güncellersiniz.

#define VER_COMPANY_NAME_STR      "LE Team"
#define VER_FILE_DESCRIPTION_STR  "Client side module"
#define VER_INTERNAL_NAME_STR     "Client"
#define VER_ORIGINAL_FILENAME_STR "Client.dll"
#define VER_PRODUCT_NAME_STR      "Client Side"
#define VER_LEGAL_COPYRIGHT_STR   "© 2025 ElessaR. All rights reserved."

// 1.2.3.4 formatı
#define VER_FILE_VERSION          1,2,3,4
#define VER_FILE_VERSION_STR      "1.2.3.4"
#define VER_PRODUCT_VERSION       VER_FILE_VERSION
#define VER_PRODUCT_VERSION_STR   VER_FILE_VERSION_STR

// Debug/Release için bayraklar
#ifdef _DEBUG
  #define VER_FILEFLAGS           0x1L  // VS_FF_DEBUG
#else
  #define VER_FILEFLAGS           0x0L
#endif

// Sabit değerler
#define VER_FILEFLAGSMASK         0x3FL
#define VER_FILEOS                0x00040004L  // VOS_NT_WINDOWS32
#define VER_FILETYPE              0x00000002L  // VFT_DLL
#define VER_FILESUBTYPE           0x00000000L  // VFT2_UNKNOWN
