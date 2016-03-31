; Build on Inno Setup 5.5.5

[Setup]
AppName=NCam Dependencies
AppId=NCamDepends
OutputBaseFilename=NCamDepends
AppVerName=NCam Dependencies 2.0
DefaultDirName=C:\Vgp\NCam
DefaultGroupName=Vgp
ChangesEnvironment=true
Compression=lzma2
SolidCompression=yes
OutputDir=..\setup

; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64


[Tasks]
Name: modifypath; Description: Add application directory to your environmental path; 

[Dirs]
Name: "{app}\bin"
          
[Files]
Source: {app}\bin\*; DestDir: {app}\backup\{code:GetBackupFolder}\bin; Check: DoBackup; Flags: external skipifsourcedoesntexist uninsneveruninstall ignoreversion recursesubdirs
Source: "..\depends\*"; DestDir: {app}\bin\; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\Uninstall"; Filename: "{app}\unins000.exe"


[CODE]
var
  overwriteConfig: Boolean;
  backupFolder: String;
  backupOrigFiles: Boolean;
    


function NextButtonClick(CurPageID: Integer): Boolean;
begin
  if (CurPageID = wpReady) then
  begin

    if (DirExists(ExpandConstant('{app}'))) then
    begin
      backupOrigFiles := (MsgBox('Deseja criar uma cópia de segurança dos arquivos existentes?', mbConfirmation, MB_YESNO) = idYes);
    end
    else
      backupOrigFiles := false;  

    if (DirExists(ExpandConstant('{app}\Config'))) then
    begin
      overwriteConfig := (MsgBox('Deseja substituir os arquivos de configuração existentes?', mbConfirmation, MB_YESNO) = idYes);
    end
    else
      overwriteConfig := true;

    BringToFrontAndRestore();
  end;
  
  Result := True;
end;


function OverwriteConfigFolder(): Boolean;
begin
  Result := overwriteConfig;
end;


function GetBackupFolder(Param: String): String;
begin
  backupFolder := GetDateTimeString('yyyy/mm/dd', '-', #0);

  Result := backupFolder;
end;


function DoBackup(): Boolean;
begin  
  Result := backupOrigFiles;
end;



//================================================
//======= Code to setup environment path =========
//== Must have "modpath.iss" into the same =======
//== directory of this stup script ===============
const 
    ModPathName = 'modifypath'; 
    ModPathType = 'system'; 

function ModPathDir(): TArrayOfString; 
begin 
    setArrayLength(Result, 1) 
    Result[0] := ExpandConstant('{app}\bin\x64'); 
end; 
#include "modpath.iss"
//================================================
//================================================
