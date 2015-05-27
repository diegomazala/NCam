; Build on Inno Setup 5.5.5

#define MyAppName "NCam Lens Table"
#define MyAppVersion "1.0"
#define MyAppPublisher "Diem - Tv Globo"
#define MyAppExeName "NCamLensTable.exe"

[Setup]
AppId={{1336FDCD-A3DA-43A6-96FC-9753EE61D034}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
OutputBaseFilename=NCamLensTable
AppVerName=NCam Lens Table 1.0
DefaultDirName={sd}\NCamLensTable
DefaultGroupName=Vgp
UninstallDisplayIcon={#MyAppName}
UninstallDisplayName={#MyAppName}
ChangesEnvironment=true
Compression=lzma2
SolidCompression=yes
OutputDir=..\setup


; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64

#define Qt5 GetEnv('QT5DIR')

[Dirs]
Name: "{app}\bin"
          
[Files]
; Copy executable
Source: "..\..\build\bin\Release_x64\NCamLensTable.exe"; DestDir: {app}\bin\; Flags: ignoreversion
; Copy ncam dependencies 
Source: "..\..\build\bin\Release_x64\LibNcamDataStreaming.dll"; DestDir: {app}\bin\; Flags: ignoreversion
Source: "..\..\build\bin\Release_x64\NCamQt.dll"; DestDir: {app}\bin\; Flags: ignoreversion
; Copy Qt5 dependencies
Source: "{#QT5}bin\icudt53.dll"; DestDir: {app}\bin\; Flags: ignoreversion
Source: "{#QT5}bin\icuin53.dll"; DestDir: {app}\bin\; Flags: ignoreversion
Source: "{#QT5}bin\icuuc53.dll"; DestDir: {app}\bin\; Flags: ignoreversion
Source: "{#QT5}bin\Qt5Core.dll"; DestDir: {app}\bin\; Flags: ignoreversion
Source: "{#QT5}bin\Qt5Gui.dll"; DestDir: {app}\bin\; Flags: ignoreversion
Source: "{#QT5}bin\Qt5Network.dll"; DestDir: {app}\bin\; Flags: ignoreversion
Source: "{#QT5}bin\Qt5OpenGL.dll"; DestDir: {app}\bin\; Flags: ignoreversion
Source: "{#QT5}bin\Qt5Widgets.dll"; DestDir: {app}\bin\; Flags: ignoreversion
Source: "{#QT5}plugins\platforms\qwindows.dll"; DestDir: {app}\bin\platforms\; Flags: ignoreversion
                                                                  

[Icons]
Name: "{app}\{#MyAppName}"; Filename: "{app}\bin\NCamLensTable.exe"; WorkingDir: "{app}\bin"
Name: "{app}\Uninstall"; Filename: "{uninstallexe}"






