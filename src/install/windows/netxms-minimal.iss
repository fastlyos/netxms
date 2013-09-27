; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#include "setup.iss"
OutputBaseFilename=netxms-1.2.9-minimal

[Components]
Name: "base"; Description: "Base Files"; Types: full compact custom; Flags: fixed
Name: "tools"; Description: "Command Line Tools"; Types: full
Name: "server"; Description: "NetXMS Server"; Types: full compact
Name: "pdb"; Description: "Install PDB files for selected components"; Types: custom

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Common files
Source: "..\..\..\ChangeLog"; DestDir: "{app}\doc"; Flags: ignoreversion; Components: base
Source: "..\..\..\Release\libnetxms.dll"; DestDir: "{app}\bin"; BeforeInstall: StopAllServices; Flags: ignoreversion; Components: base
Source: "..\..\..\Release\libnetxms.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: base and pdb
Source: "..\..\..\Release\libexpat.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: base
Source: "..\..\..\Release\libexpat.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: base and pdb
Source: "..\..\..\Release\libtre.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: base
Source: "..\..\..\Release\libtre.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: base and pdb
Source: "..\..\..\Release\nxzlib.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: base
Source: "..\..\..\Release\nxzlib.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: base and pdb
; Executables and DLLs shared between different components (server, console, etc.)
Source: "..\..\..\Release\libnxcl.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: tools
Source: "..\..\..\Release\libnxcl.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: tools and pdb
Source: "..\..\..\Release\libnxmap.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server tools
Source: "..\..\..\Release\libnxmap.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: (server or tools) and pdb
Source: "..\..\..\Release\libnxsnmp.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\libnxsnmp.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\libnxsl.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\libnxsl.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\nxscript.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxconfig.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxinstall.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
; Server files
Source: "..\..\..\Release\appagent.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\appagent.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\nxsqlite.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxsqlite.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\libnxlp.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\libnxlp.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\libnxdb.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\libnxdb.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\libnxsrv.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\libnxsrv.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\nxcore.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxcore.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\netxmsd.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\netxmsd.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\mysql.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\mysql.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\mssql.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\mssql.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\odbc.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\odbc.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\pgsql.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\pgsql.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\sqlite.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\sqlite.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\oracle.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\oracle.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\informix.ddr"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\informix.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\generic.sms"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\generic.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\nxagent.sms"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxagent.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\portech.sms"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\portech.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\dbemu.sms"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\dbemu.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\nxaction.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxadm.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxdbmgr.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxencpasswd.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxget.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxget.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\nxsnmpget.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxsnmpwalk.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxsnmpset.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxupload.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxmibc.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\nxagentd.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\winnt.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\winnt.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\winperf.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\winperf.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\wmi.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\wmi.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\ping.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\ping.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\portcheck.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\portcheck.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\ecs.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\ecs.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\logwatch.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\logwatch.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\ups.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\ups.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\odbcquery.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\odbcquery.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\informix.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\informix.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\oracle.nsm"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\oracle.pdb"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\avaya-ers.dll"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\avaya-ers.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\baystack.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\baystack.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\cisco.dll"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\cisco.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\cat2900xl.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\cat2900xl.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\catalyst.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\catalyst.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\cisco-esw.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\cisco-esw.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\dell-pwc.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\dell-pwc.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\ers8000.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\ers8000.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\netscreen.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\netscreen.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\ntws.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\ntws.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\procurve.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\procurve.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\Release\symbol-ws.ndd"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server
Source: "..\..\..\Release\symbol-ws.pdb"; DestDir: "{app}\lib\ndd"; Flags: ignoreversion; Components: server and pdb
Source: "..\..\..\sql\dbinit_mssql.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\sql\dbinit_mysql.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\sql\dbinit_oracle.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\sql\dbinit_pgsql.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\sql\dbinit_sqlite.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\sql\dbschema_mssql.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\sql\dbschema_mysql.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\sql\dbschema_oracle.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\sql\dbschema_pgsql.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\sql\dbschema_sqlite.sql"; DestDir: "{app}\lib\sql"; Flags: ignoreversion; Components: server
Source: "..\..\..\contrib\mibs\*.txt"; DestDir: "{app}\var\mibs"; Flags: ignoreversion; Components: server
Source: "..\..\..\contrib\netxmsd.conf-dist"; DestDir: "{app}\etc"; Flags: ignoreversion; Components: server
Source: "..\..\..\contrib\nxagentd.conf-dist"; DestDir: "{app}\etc"; Flags: ignoreversion; Components: server
Source: "..\..\..\images\*"; DestDir: "{app}\var\images"; Flags: ignoreversion; Components: server
;Source: "..\..\java\report-generator\target\report-generator.jar"; DestDir: "{app}\lib\java"; Flags: ignoreversion; Components: server
; Command-line tools files
Source: "..\..\..\Release\nxalarm.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: tools
Source: "..\..\..\Release\nxsms.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: tools
Source: "..\..\..\Release\nxevent.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: tools
Source: "..\..\..\Release\nxpush.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: tools
Source: "..\..\..\Release\nxapush.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: tools
Source: "..\..\..\Release\nxappget.exe"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: tools
; Third party files
Source: "Files\libeay32.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: base
Source: "Files\ssleay32.dll"; DestDir: "{app}\bin"; Flags: ignoreversion; Components: base
; Install-time files
Source: "Files\vcredist_x86.exe"; DestDir: "{app}\var"; DestName: "vcredist.exe"; Flags: ignoreversion deleteafterinstall; Components: base
Source: "Files\rm.exe"; DestDir: "{app}\var"; Flags: ignoreversion deleteafterinstall; Components: base

#include "icons-minimal.iss"
#include "common.iss"

