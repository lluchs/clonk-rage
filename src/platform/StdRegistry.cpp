/* Copyright (C) 1998-2000  Matthes Bender  RedWolf Design */

/* Some wrappers for easier access to the Windows registry */

#include <Standard.h>
#include <StdRegistry.h>

#ifdef _WIN32

#include <stdio.h>

BOOL DeleteRegistryValue(const char *szSubKey, const char *szValueName) {
  return DeleteRegistryValue(HKEY_CURRENT_USER, szSubKey, szValueName);
}

BOOL DeleteRegistryValue(HKEY hKey, const char *szSubKey,
                         const char *szValueName) {
  long qerr;
  HKEY ckey;
  // Open the key
  if ((qerr = RegOpenKeyEx(hKey, szSubKey, 0, KEY_ALL_ACCESS, &ckey)) !=
      ERROR_SUCCESS)
    return FALSE;
  // Delete the key
  if ((qerr = RegDeleteValue(ckey, szValueName)) != ERROR_SUCCESS) return FALSE;
  // Close the key
  RegCloseKey(ckey);
  // Success
  return TRUE;
}

BOOL SetRegistryDWord(const char *szSubKey, const char *szValueName,
                      DWORD dwValue) {
  return SetRegistryDWord(HKEY_CURRENT_USER, szSubKey, szValueName, dwValue);
}

BOOL GetRegistryDWord(const char *szSubKey, const char *szValueName,
                      DWORD *lpdwValue) {
  return GetRegistryDWord(HKEY_CURRENT_USER, szSubKey, szValueName, lpdwValue);
}

BOOL GetRegistryDWord(HKEY hKey, const char *szSubKey, const char *szValueName,
                      DWORD *lpdwValue) {
  long qerr;
  HKEY ckey;
  DWORD valtype;
  DWORD valsize = sizeof(DWORD);

  // Open the key
  if ((qerr = RegOpenKeyEx(hKey, szSubKey, 0, KEY_READ, &ckey)) !=
      ERROR_SUCCESS)
    return FALSE;

  // Get the value
  if ((qerr = RegQueryValueEx(ckey, szValueName, NULL, &valtype,
                              (BYTE *)lpdwValue, &valsize)) != ERROR_SUCCESS) {
    RegCloseKey(ckey);
    return FALSE;
  }

  // Close the key
  RegCloseKey(ckey);

  if (valtype != REG_DWORD) return FALSE;

  return TRUE;
}

BOOL SetRegistryDWord(HKEY hKey, const char *szSubKey, const char *szValueName,
                      DWORD dwValue) {
  long qerr;
  HKEY ckey;
  DWORD disposition;
  // Open the key
  if ((qerr = RegCreateKeyEx(hKey, szSubKey, 0, "", REG_OPTION_NON_VOLATILE,
                             KEY_ALL_ACCESS, NULL, &ckey, &disposition)) !=
      ERROR_SUCCESS)
    return FALSE;
  // Set the value
  if ((qerr = RegSetValueEx(ckey, szValueName, 0, REG_DWORD, (BYTE *)&dwValue,
                            sizeof(dwValue))) != ERROR_SUCCESS) {
    RegCloseKey(ckey);
    return FALSE;
  }

  // Close the key
  RegCloseKey(ckey);
  // Success
  return TRUE;
}

BOOL GetRegistryString(const char *szSubKey, const char *szValueName,
                       char *sValue, DWORD dwValSize) {
  long qerr;
  HKEY ckey;
  DWORD valtype;

  // Open the key
  if ((qerr = RegOpenKeyEx(HKEY_CURRENT_USER, szSubKey, 0, KEY_READ, &ckey)) !=
      ERROR_SUCCESS)
    return FALSE;

  // Get the value
  if ((qerr = RegQueryValueEx(ckey, szValueName, NULL, &valtype, (BYTE *)sValue,
                              &dwValSize)) != ERROR_SUCCESS) {
    RegCloseKey(ckey);
    return FALSE;
  }

  // Close the key
  RegCloseKey(ckey);

  if (valtype != REG_SZ) return FALSE;

  return TRUE;
}

BOOL SetRegistryString(const char *szSubKey, const char *szValueName,
                       const char *szValue) {
  long qerr;
  HKEY ckey;
  DWORD disposition;

  // Open the key
  if ((qerr = RegCreateKeyEx(HKEY_CURRENT_USER, szSubKey, 0, "",
                             REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                             &ckey, &disposition)) != ERROR_SUCCESS)
    return FALSE;

  // Set the value
  if ((qerr = RegSetValueEx(ckey, szValueName, 0, REG_SZ, (BYTE *)szValue,
                            SLen(szValue) + 1)) != ERROR_SUCCESS) {
    RegCloseKey(ckey);
    return FALSE;
  }

  // Close the key
  RegCloseKey(ckey);

  return TRUE;
}

BOOL DeleteRegistryKey(HKEY hKey, const char *szSubKey) {
  HKEY ckey;
  // Open the key
  if (RegOpenKeyEx(hKey, szSubKey, 0, KEY_ALL_ACCESS, &ckey) != ERROR_SUCCESS)
    return FALSE;
  // Delete all subkeys
  char strChild[1024 + 1];
  while (RegEnumKey(ckey, 0, strChild, 1024) == ERROR_SUCCESS)
    if (!DeleteRegistryKey(ckey, strChild)) return FALSE;
  // Close the key
  RegCloseKey(ckey);

  // Delete the key
  if (RegDeleteKey(hKey, szSubKey) != ERROR_SUCCESS) return FALSE;
  // Success
  return TRUE;
}

BOOL DeleteRegistryKey(const char *szSubKey) {
  return DeleteRegistryKey(HKEY_CURRENT_USER, szSubKey);
}

BOOL SetRegClassesRoot(const char *szSubKey, const char *szValueName,
                       const char *szStringValue) {
  long qerr;
  HKEY ckey;
  DWORD disposition;

  // Open the key
  if ((qerr = RegCreateKeyEx(HKEY_CLASSES_ROOT, szSubKey, 0, "",
                             REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                             &ckey, &disposition)) != ERROR_SUCCESS)
    return FALSE;

  // Set the value
  if ((qerr = RegSetValueEx(ckey, szValueName, 0, REG_SZ, (BYTE *)szStringValue,
                            SLen(szStringValue) + 1)) != ERROR_SUCCESS) {
    RegCloseKey(ckey);
    return FALSE;
  }

  // Close the key
  RegCloseKey(ckey);

  return TRUE;
}

BOOL SetRegClassesRootString(const char *szSubKey, const char *szValueName,
                             const char *szValue) {
  long qerr;
  HKEY ckey;
  DWORD disposition;

  // Open the key
  if ((qerr = RegCreateKeyEx(HKEY_CLASSES_ROOT, szSubKey, 0, "",
                             REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                             &ckey, &disposition)) != ERROR_SUCCESS)
    return FALSE;

  // Set the value
  if ((qerr = RegSetValueEx(ckey, szValueName, 0, REG_SZ, (BYTE *)szValue,
                            SLen(szValue) + 1)) != ERROR_SUCCESS) {
    RegCloseKey(ckey);
    return FALSE;
  }

  // Close the key
  RegCloseKey(ckey);

  return TRUE;
}

BOOL SetRegShell(const char *szClassName, const char *szShellName,
                 const char *szShellCaption, const char *szCommand,
                 BOOL fMakeDefault) {
  char szKeyName[256 + 1];
  // Set shell caption
  sprintf(szKeyName, "%s\\Shell\\%s", szClassName, szShellName);
  if (!SetRegClassesRoot(szKeyName, NULL, szShellCaption)) return FALSE;
  // Set shell command
  sprintf(szKeyName, "%s\\Shell\\%s\\Command", szClassName, szShellName);
  if (!SetRegClassesRoot(szKeyName, NULL, szCommand)) return FALSE;
  // Set as default command
  if (fMakeDefault) {
    sprintf(szKeyName, "%s\\Shell", szClassName);
    if (!SetRegClassesRoot(szKeyName, NULL, szShellName)) return FALSE;
  }
  return TRUE;
}

BOOL RemoveRegShell(const char *szClassName, const char *szShellName) {
  char strKey[256 + 1];
  sprintf(strKey, "%s\\Shell\\%s", szClassName, szShellName);
  if (!DeleteRegistryKey(HKEY_CLASSES_ROOT, strKey)) return FALSE;
  return TRUE;
}

BOOL SetRegFileClass(const char *szClassRoot, const char *szExtension,
                     const char *szClassName, const char *szIconPath,
                     int iIconNum, const char *szContentType) {
  char keyname[100];
  char iconpath[512];
  // Create root class entry
  if (!SetRegClassesRoot(szClassRoot, NULL, szClassName)) return FALSE;
  // Set root class icon
  sprintf(keyname, "%s\\DefaultIcon", szClassRoot);
  sprintf(iconpath, "%s,%d", szIconPath, iIconNum);
  if (!SetRegClassesRoot(keyname, NULL, iconpath)) return FALSE;
  // Set extension map entry
  sprintf(keyname, ".%s", szExtension);
  if (!SetRegClassesRoot(keyname, NULL, szClassRoot)) return FALSE;
  // Set extension content type
  sprintf(keyname, ".%s", szExtension);
  if (!SetRegClassesRootString(keyname, "Content Type", szContentType))
    return FALSE;
  // Success
  return TRUE;
}

//------------------------------ Window Position
//------------------------------------------

BOOL StoreWindowPosition(HWND hwnd, const char *szWindowName,
                         const char *szSubKey, BOOL fStoreSize) {
  RECT winpos;
  char regstr[100];
  if (IsZoomed(hwnd))
    return SetRegistryString(szSubKey, szWindowName, "Maximized");
  if (IsIconic(hwnd))
    return SetRegistryString(szSubKey, szWindowName, "Minimized");
  if (!GetWindowRect(hwnd, &winpos)) return FALSE;
  if (fStoreSize)
    sprintf(regstr, "%i,%i,%i,%i", winpos.left, winpos.top,
            winpos.right - winpos.left, winpos.bottom - winpos.top);
  else
    sprintf(regstr, "%i,%i", winpos.left, winpos.top);
  return SetRegistryString(szSubKey, szWindowName, regstr);
}

BOOL RestoreWindowPosition(HWND hwnd, const char *szWindowName,
                           const char *szSubKey, BOOL fHidden) {
  char regstr[100], buffer2[5];
  int x, y, wdt, hgt;
  BOOL fSetSize = TRUE;
  // No position stored: cannot restore
  if (!GetRegistryString(szSubKey, szWindowName, regstr, 100)) return FALSE;
  if (SEqual(regstr, "Maximized"))
    return ShowWindow(hwnd, SW_MAXIMIZE | SW_NORMAL);
  if (SEqual(regstr, "Minimized"))
    return ShowWindow(hwnd, SW_MINIMIZE | SW_NORMAL);
  SCopySegment(regstr, 0, buffer2, ',', 4);
  sscanf(buffer2, "%i", &x);
  SCopySegment(regstr, 1, buffer2, ',', 4);
  sscanf(buffer2, "%i", &y);
  if (SCopySegment(regstr, 2, buffer2, ',', 4))
    sscanf(buffer2, "%i", &wdt);
  else
    fSetSize = FALSE;
  if (SCopySegment(regstr, 3, buffer2, ',', 4))
    sscanf(buffer2, "%i", &hgt);
  else
    fSetSize = FALSE;
  if (!fSetSize) {
    RECT winpos;
    if (!GetWindowRect(hwnd, &winpos)) return FALSE;
    wdt = winpos.right - winpos.left;
    hgt = winpos.bottom - winpos.top;
  }
  // Move window
  if (!MoveWindow(hwnd, x, y, wdt, hgt, TRUE)) return FALSE;
  // Hide window
  if (fHidden) return ShowWindow(hwnd, SW_HIDE);
  // Show window
  return ShowWindow(hwnd, SW_NORMAL);
}

//------------------------------ Registry compiler
//------------------------------------------

// *** StdCompilerConfigWrite

StdCompilerConfigWrite::StdCompilerConfigWrite(HKEY hRoot, const char *szPath)
    : pKey(new Key()), iDepth(0) {
  pKey->Name = szPath;
  pKey->Handle = 0;
  CreateKey(hRoot);
}

StdCompilerConfigWrite::~StdCompilerConfigWrite() {
  assert(!iDepth);
  if (pKey->Handle) RegCloseKey(pKey->Handle);
  delete pKey;
}

bool StdCompilerConfigWrite::Name(const char *szName) {
  // Open parent key (if not already done so)
  CreateKey();
  // Push new subkey onto the stack
  Key *pnKey = new Key();
  pnKey->Handle = 0;
  pnKey->Name = szName;
  pnKey->Parent = pKey;
  pKey = pnKey;
  iDepth++;
  return true;
}

void StdCompilerConfigWrite::NameEnd(bool fBreak) {
  assert(iDepth);
  // Close current key
  if (pKey->Handle) RegCloseKey(pKey->Handle);
  // Pop
  Key *poKey = pKey;
  pKey = poKey->Parent;
  delete poKey;
  iDepth--;
}

bool StdCompilerConfigWrite::FollowName(const char *szName) {
  NameEnd();
  return Name(szName);
}

bool StdCompilerConfigWrite::Default(const char *szName) {
  // Open parent
  CreateKey();
  // Remove key/value (failsafe)
  RegDeleteKey(pKey->Handle, szName);
  RegDeleteValue(pKey->Handle, szName);
  // Handled
  return true;
}

bool StdCompilerConfigWrite::Seperator(Sep eSep) {
  excCorrupt("Seperators not supported by registry compiler!");
  return false;
}

void StdCompilerConfigWrite::DWord(int32_t &rInt) { WriteDWord(rInt); }
void StdCompilerConfigWrite::DWord(uint32_t &rInt) { WriteDWord(rInt); }
void StdCompilerConfigWrite::Word(int16_t &rInt) { WriteDWord(rInt); }
void StdCompilerConfigWrite::Word(uint16_t &rInt) { WriteDWord(rInt); }
void StdCompilerConfigWrite::Byte(int8_t &rInt) { WriteDWord(rInt); }
void StdCompilerConfigWrite::Byte(uint8_t &rInt) { WriteDWord(rInt); }
void StdCompilerConfigWrite::Boolean(bool &rBool) { WriteDWord(rBool ? 1 : 0); }
void StdCompilerConfigWrite::Character(char &rChar) {
  WriteString(FormatString("%c", rChar).getData());
}

void StdCompilerConfigWrite::String(char *szString, size_t iMaxLength,
                                    RawCompileType eType) {
  WriteString(szString);
}

void StdCompilerConfigWrite::String(char **pszString, RawCompileType eType) {
  WriteString(pszString ? *pszString : "");
}

void StdCompilerConfigWrite::Raw(void *pData, size_t iSize,
                                 RawCompileType eType) {
  excCorrupt("Raw values aren't supported for registry compilers!");
}

void StdCompilerConfigWrite::Begin() { assert(!iDepth); }

void StdCompilerConfigWrite::End() { assert(!iDepth); }

void StdCompilerConfigWrite::CreateKey(HKEY hParent) {
  // Already open?
  if (pKey->Handle) return;
  // Open/Create registry key
  if (RegCreateKeyEx(hParent ? hParent : pKey->Parent->Handle,
                     pKey->Name.getData(), 0, "", REG_OPTION_NON_VOLATILE,
                     KEY_WRITE, NULL, &pKey->Handle, NULL) != ERROR_SUCCESS)
    excCorrupt(0,
               FormatString("Could not create key %s!", pKey->Name.getData()));
}

void StdCompilerConfigWrite::WriteDWord(uint32_t iVal) {
  // Set the value
  if (RegSetValueEx(pKey->Parent->Handle, pKey->Name.getData(), 0, REG_DWORD,
                    reinterpret_cast<const BYTE *>(&iVal),
                    sizeof(iVal)) != ERROR_SUCCESS)
    excCorrupt(0,
               FormatString("Could not write key %s!", pKey->Name.getData()));
}

void StdCompilerConfigWrite::WriteString(const char *szString) {
  // Set the value
  if (RegSetValueEx(pKey->Parent->Handle, pKey->Name.getData(), 0, REG_SZ,
                    reinterpret_cast<const BYTE *>(szString),
                    strlen(szString) + 1) != ERROR_SUCCESS)
    excCorrupt(0,
               FormatString("Could not write key %s!", pKey->Name.getData()));
}

// *** StdCompilerConfigRead

StdCompilerConfigRead::StdCompilerConfigRead(HKEY hRoot, const char *szPath)
    : pKey(new Key()), iDepth(0) {
  pKey->Name = szPath;
  pKey->Virtual = false;
  // Open root
  if (RegOpenKeyEx(hRoot, szPath, 0, KEY_READ, &pKey->Handle) != ERROR_SUCCESS)
    pKey->Handle = 0;
}

StdCompilerConfigRead::~StdCompilerConfigRead() {
  assert(!iDepth);
  if (pKey->Handle) RegCloseKey(pKey->Handle);
  delete pKey;
}

bool StdCompilerConfigRead::Name(const char *szName) {
  bool fFound = true;
  // Try to open registry key
  HKEY hSubKey;
  DWORD dwType = 0;
  if (RegOpenKeyEx(pKey->Handle, szName, 0, KEY_READ, &hSubKey) !=
      ERROR_SUCCESS) {
    hSubKey = 0;
    // Try to query value (exists?)
    if (RegQueryValueEx(pKey->Handle, szName, 0, &dwType, NULL, NULL) !=
        ERROR_SUCCESS)
      fFound = false;
  }
  // Push new subkey on the stack
  Key *pnKey = new Key();
  pnKey->Name = szName;
  pnKey->Handle = hSubKey;
  pnKey->Parent = pKey;
  pnKey->Virtual = !fFound;
  pnKey->Type = dwType;
  pKey = pnKey;
  iDepth++;
  return fFound;
}

void StdCompilerConfigRead::NameEnd(bool fBreak) {
  assert(iDepth);
  // Close current key
  if (pKey->Handle) RegCloseKey(pKey->Handle);
  // Pop
  Key *poKey = pKey;
  pKey = poKey->Parent;
  delete poKey;
  iDepth--;
}

bool StdCompilerConfigRead::FollowName(const char *szName) {
  NameEnd();
  return Name(szName);
}

bool StdCompilerConfigRead::Seperator(Sep eSep) {
  excCorrupt(0, "Seperators not supported by registry compiler!");
  return false;
}

void StdCompilerConfigRead::DWord(int32_t &rInt) {
  rInt = int32_t(ReadDWord());
}
void StdCompilerConfigRead::DWord(uint32_t &rInt) { rInt = ReadDWord(); }
void StdCompilerConfigRead::Word(int16_t &rInt) { rInt = int16_t(ReadDWord()); }
void StdCompilerConfigRead::Word(uint16_t &rInt) {
  rInt = uint16_t(ReadDWord());
}
void StdCompilerConfigRead::Byte(int8_t &rInt) { rInt = int8_t(ReadDWord()); }
void StdCompilerConfigRead::Byte(uint8_t &rInt) { rInt = uint8_t(ReadDWord()); }
void StdCompilerConfigRead::Boolean(bool &rBool) {
  try {
    StdStrBuf szVal = ReadString();
    rBool = (szVal == "true");
  } catch (NotFoundException *pExc) {
    delete pExc;
    uint32_t iVal = ReadDWord();
    rBool = !!iVal;
  }
}
void StdCompilerConfigRead::Character(char &rChar) {
  try {
    StdStrBuf szVal = ReadString();
    rChar = *szVal.getData();
  } catch (NotFoundException *pExc) {
    delete pExc;
    uint32_t iVal = ReadDWord();
    rChar = char(iVal);
  }
}

void StdCompilerConfigRead::String(char *szString, size_t iMaxLength,
                                   RawCompileType eType) {
  StdStrBuf Result = ReadString();
  SCopy(Result.getData(), szString, iMaxLength);
}

void StdCompilerConfigRead::String(char **pszString, RawCompileType eType) {
  *pszString = ReadString().GrabPointer();
}

void StdCompilerConfigRead::Raw(void *pData, size_t iSize,
                                RawCompileType eType) {
  excCorrupt(0, "Raw values aren't supported for registry compilers!");
}

void StdCompilerConfigRead::Begin() { assert(!iDepth); }

void StdCompilerConfigRead::End() { assert(!iDepth); }

uint32_t StdCompilerConfigRead::ReadDWord() {
  // Virtual key?
  if (pKey->Virtual) {
    excNotFound("Could not read value %s! Parent key doesn't exist!",
                pKey->Name.getData());
    return 0;
  }
  // Wrong type?
  if (pKey->Type != REG_DWORD && pKey->Type != REG_DWORD_LITTLE_ENDIAN) {
    excNotFound("Wrong value type!");
    return 0;
  }
  // Read
  uint32_t iVal;
  DWORD iSize = sizeof(iVal);
  if (RegQueryValueEx(pKey->Parent->Handle, pKey->Name.getData(), 0, NULL,
                      reinterpret_cast<LPBYTE>(&iVal),
                      &iSize) != ERROR_SUCCESS) {
    excNotFound("Could not read value %s!", pKey->Name.getData());
    return 0;
  }
  // Check size
  if (iSize != sizeof(iVal)) {
    excCorrupt("Wrong size of a DWord!");
    return 0;
  }
  // Return
  return iVal;
}

StdStrBuf StdCompilerConfigRead::ReadString() {
  // Virtual key?
  if (pKey->Virtual) {
    excNotFound("Could not read value %s! Parent key doesn't exist!",
                pKey->Name.getData());
    return StdStrBuf();
  }
  // Wrong type?
  if (pKey->Type != REG_SZ) {
    excNotFound("Wrong value type!");
    return StdStrBuf();
  }
  // Get size of string
  DWORD iSize;
  if (RegQueryValueEx(pKey->Parent->Handle, pKey->Name.getData(), 0, NULL, NULL,
                      &iSize) != ERROR_SUCCESS) {
    excNotFound("Could not read value %s!", pKey->Name.getData());
    return StdStrBuf();
  }
  // Allocate string
  StdStrBuf Result;
  Result.SetLength(iSize - 1);
  // Read
  if (RegQueryValueEx(pKey->Parent->Handle, pKey->Name.getData(), 0, NULL,
                      reinterpret_cast<BYTE *>(Result.getMData()),
                      &iSize) != ERROR_SUCCESS) {
    excNotFound("Could not read value %s!", pKey->Name.getData());
    return StdStrBuf();
  }
  // Check size
  if (strlen(Result.getData()) + 1 != iSize) {
    excCorrupt("Wrong size of a string!");
    return StdStrBuf();
  }
  return Result;
}

#endif  // _WIN32
