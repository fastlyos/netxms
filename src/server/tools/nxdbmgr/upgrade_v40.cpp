/*
** nxdbmgr - NetXMS database manager
** Copyright (C) 2020 Raden Solutions
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: upgrade_v40.cpp
**
**/

#include "nxdbmgr.h"

/**
 * Upgrade from 40.3 to 40.4
 */
static bool H_UpgradeFromV3()
{
   if (GetSchemaLevelForMajorVersion(34) < 14)
   {
      CHK_EXEC(CreateConfigParam(_T("DBWriter.HouseKeeperInterlock"), _T("0"), _T("Controls if server should block background write of collected performance data while housekeeper deletes expired records."), nullptr, 'C', true, false, false, false));

      static const TCHAR *batch =
            _T("INSERT INTO config_values (var_name,var_value,var_description) VALUES ('DBWriter.HouseKeeperInterlock','0','Auto')\n")
            _T("INSERT INTO config_values (var_name,var_value,var_description) VALUES ('DBWriter.HouseKeeperInterlock','1','Off')\n")
            _T("INSERT INTO config_values (var_name,var_value,var_description) VALUES ('DBWriter.HouseKeeperInterlock','2','On')\n")
            _T("UPDATE config_values SET var_name='LDAP.UserDeleteAction' WHERE var_name='LdapUserDeleteAction'\n")
            _T("<END>");
      CHK_EXEC(SQLBatch(batch));

      CHK_EXEC(SetSchemaLevelForMajorVersion(34, 14));
   }
   CHK_EXEC(SetMinorSchemaVersion(4));
   return true;
}

/**
 * Upgrade from 40.2 to 40.3
 */
static bool H_UpgradeFromV2()
{
   if (GetSchemaLevelForMajorVersion(34) < 13)
   {
      CHK_EXEC(SQLQuery(_T("INSERT INTO metadata (var_name,var_value) VALUES ('PruneCustomAttributes', '1')")));
      CHK_EXEC(SetSchemaLevelForMajorVersion(34, 13));
   }
   CHK_EXEC(SetMinorSchemaVersion(3));
   return true;
}

/**
 * Upgrade from 40.1 to 40.2
 */
static bool H_UpgradeFromV1()
{
   if (g_dbSyntax == DB_SYNTAX_TSDB)
   {
      CHK_EXEC(SQLQuery(_T("ALTER TABLE alarms RENAME TO old_alarms")));
      CHK_EXEC(CreateTable(
            _T("CREATE TABLE alarms (")
            _T("   alarm_id integer not null,")
            _T("   parent_alarm_id integer not null,")
            _T("   alarm_state integer not null,")
            _T("   hd_state integer not null,")
            _T("   hd_ref varchar(63) null,")
            _T("   creation_time integer not null,")
            _T("   last_change_time integer not null,")
            _T("   rule_guid varchar(36) null,")
            _T("   source_object_id integer not null,")
            _T("   zone_uin integer not null,")
            _T("   source_event_code integer not null,")
            _T("   source_event_id $SQL:INT64 not null,")
            _T("   dci_id integer not null,")
            _T("   message varchar(2000) null,")
            _T("   original_severity integer not null,")
            _T("   current_severity integer not null,")
            _T("   repeat_count integer not null,")
            _T("   alarm_key varchar(255) null,")
            _T("   ack_by integer not null,")
            _T("   resolved_by integer not null,")
            _T("   term_by integer not null,")
            _T("   timeout integer not null,")
            _T("   timeout_event integer not null,")
            _T("   ack_timeout integer not null,")
            _T("   alarm_category_ids varchar(255) null,")
            _T("   event_tags varchar(2000) null,")
            _T("   rca_script_name varchar(255) null,")
            _T("   impact varchar(1000) null,")
            _T("   PRIMARY KEY(alarm_id))")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_alarms_source_object_id ON alarms(source_object_id)")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_alarms_last_change_time ON alarms(last_change_time)")));
      CHK_EXEC(SQLQuery(_T("INSERT INTO alarms (alarm_id,parent_alarm_id,alarm_state,hd_state,hd_ref,creation_time,last_change_time,rule_guid,source_object_id,zone_uin,source_event_code,source_event_id,dci_id,message,original_severity,current_severity,repeat_count,alarm_key,ack_by,resolved_by,term_by,timeout,timeout_event,ack_timeout,alarm_category_ids,event_tags,rca_script_name,impact) SELECT alarm_id,parent_alarm_id,alarm_state,hd_state,hd_ref,creation_time,last_change_time,rule_guid,source_object_id,zone_uin,source_event_code,source_event_id,dci_id,message,original_severity,current_severity,repeat_count,alarm_key,ack_by,resolved_by,term_by,timeout,timeout_event,ack_timeout,alarm_category_ids,event_tags,rca_script_name,impact FROM old_alarms")));
      CHK_EXEC(SQLQuery(_T("DROP TABLE old_alarms CASCADE")));

      CHK_EXEC(SQLQuery(_T("ALTER TABLE event_log RENAME TO event_log_v40_2")));
      CHK_EXEC(SQLQuery(_T("DROP INDEX IF EXISTS idx_event_log_event_timestamp")));
      CHK_EXEC(SQLQuery(_T("DROP INDEX IF EXISTS idx_event_log_source")));
      CHK_EXEC(SQLQuery(_T("DROP INDEX IF EXISTS idx_event_log_root_id")));
      CHK_EXEC(CreateTable(
            _T("CREATE TABLE event_log (")
            _T("   event_id $SQL:INT64 not null,")
            _T("   event_code integer not null,")
            _T("   event_timestamp timestamptz not null,")
            _T("   origin integer not null,")
            _T("   origin_timestamp integer not null,")
            _T("   event_source integer not null,")
            _T("   zone_uin integer not null,")
            _T("   dci_id integer not null,")
            _T("   event_severity integer not null,")
            _T("   event_message varchar(2000) null,")
            _T("   event_tags varchar(2000) null,")
            _T("   root_event_id $SQL:INT64 not null,")
            _T("   raw_data $SQL:TEXT null,")
            _T("   PRIMARY KEY(event_id,event_timestamp))")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_event_log_event_timestamp ON event_log(event_timestamp)")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_event_log_source ON event_log(event_source)")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_event_log_root_id ON event_log(root_event_id) WHERE root_event_id > 0")));
      CHK_EXEC(SQLQuery(_T("SELECT create_hypertable('event_log', 'event_timestamp', chunk_time_interval => interval '86400 seconds')")));

      CHK_EXEC(SQLQuery(_T("ALTER TABLE syslog RENAME TO syslog_v40_2")));
      CHK_EXEC(SQLQuery(_T("DROP INDEX IF EXISTS idx_syslog_msg_timestamp")));
      CHK_EXEC(SQLQuery(_T("DROP INDEX IF EXISTS idx_syslog_source")));
      CHK_EXEC(CreateTable(
            _T("CREATE TABLE syslog (")
            _T("   msg_id $SQL:INT64 not null,")
            _T("   msg_timestamp timestamptz not null,")
            _T("   facility integer not null,")
            _T("   severity integer not null,")
            _T("   source_object_id integer not null,")
            _T("   zone_uin integer not null,")
            _T("   hostname varchar(127) null,")
            _T("   msg_tag varchar(32) null,")
            _T("   msg_text $SQL:TEXT null,")
            _T("   PRIMARY KEY(msg_id,msg_timestamp))")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_syslog_msg_timestamp ON syslog(msg_timestamp)")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_syslog_source ON syslog(source_object_id)")));
      CHK_EXEC(SQLQuery(_T("SELECT create_hypertable('syslog', 'msg_timestamp', chunk_time_interval => interval '86400 seconds')")));

      CHK_EXEC(SQLQuery(_T("ALTER TABLE snmp_trap_log RENAME TO snmp_trap_log_v40_2")));
      CHK_EXEC(SQLQuery(_T("DROP INDEX IF EXISTS idx_snmp_trap_log_tt")));
      CHK_EXEC(SQLQuery(_T("DROP INDEX IF EXISTS idx_snmp_trap_log_oid")));
      CHK_EXEC(CreateTable(
            _T("CREATE TABLE snmp_trap_log (")
            _T("   trap_id $SQL:INT64 not null,")
            _T("   trap_timestamp timestamptz not null,")
            _T("   ip_addr varchar(48) not null,")
            _T("   object_id integer not null,")
            _T("   zone_uin integer not null,")
            _T("   trap_oid varchar(255) not null,")
            _T("   trap_varlist $SQL:TEXT null,")
            _T("   PRIMARY KEY(trap_id,trap_timestamp))")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_snmp_trap_log_tt ON snmp_trap_log(trap_timestamp)")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_snmp_trap_log_oid ON snmp_trap_log(object_id)")));
      CHK_EXEC(SQLQuery(_T("SELECT create_hypertable('snmp_trap_log', 'trap_timestamp', chunk_time_interval => interval '86400 seconds')")));

      RegisterOnlineUpgrade(40, 2);
   }
   else
   {
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_alarms_source_object_id ON alarms(source_object_id)")));
      CHK_EXEC(SQLQuery(_T("CREATE INDEX idx_alarms_last_change_time ON alarms(last_change_time)")));
   }
   CHK_EXEC(SetMinorSchemaVersion(2));
   return true;
}

/**
 * Upgrade from 40.0 to 40.1
 */
static bool H_UpgradeFromV0()
{
   CHK_EXEC(SQLQuery(_T("UPDATE config SET need_server_restart=0 WHERE var_name='CheckTrustedNodes'")));
   CHK_EXEC(SQLQuery(_T("UPDATE config SET need_server_restart=0 WHERE var_name='NXSL.EnableContainerFunctions'")));
   CHK_EXEC(SetMinorSchemaVersion(1));
   return true;
}

/**
 * Upgrade map
 */
static struct
{
   int version;
   int nextMajor;
   int nextMinor;
   bool (*upgradeProc)();
} s_dbUpgradeMap[] =
{
   { 3,  40, 4,  H_UpgradeFromV3  },
   { 2,  40, 3,  H_UpgradeFromV2  },
   { 1,  40, 2,  H_UpgradeFromV1  },
   { 0,  40, 1,  H_UpgradeFromV0  },
   { 0,  0,  0,  nullptr          }
};

/**
 * Upgrade database to new version
 */
bool MajorSchemaUpgrade_V40()
{
   INT32 major, minor;
   if (!DBGetSchemaVersion(g_dbHandle, &major, &minor))
      return false;

   while((major == 40) && (minor < DB_SCHEMA_VERSION_V40_MINOR))
   {
      // Find upgrade procedure
      int i;
      for(i = 0; s_dbUpgradeMap[i].upgradeProc != nullptr; i++)
         if (s_dbUpgradeMap[i].version == minor)
            break;
      if (s_dbUpgradeMap[i].upgradeProc == nullptr)
      {
         _tprintf(_T("Unable to find upgrade procedure for version 40.%d\n"), minor);
         return false;
      }
      _tprintf(_T("Upgrading from version 40.%d to %d.%d\n"), minor, s_dbUpgradeMap[i].nextMajor, s_dbUpgradeMap[i].nextMinor);
      DBBegin(g_dbHandle);
      if (s_dbUpgradeMap[i].upgradeProc())
      {
         DBCommit(g_dbHandle);
         if (!DBGetSchemaVersion(g_dbHandle, &major, &minor))
            return false;
      }
      else
      {
         _tprintf(_T("Rolling back last stage due to upgrade errors...\n"));
         DBRollback(g_dbHandle);
         return false;
      }
   }
   return true;
}
