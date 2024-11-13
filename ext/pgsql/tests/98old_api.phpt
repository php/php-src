--TEST--
PostgreSQL old api
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = "table_98old_api";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");
pg_query($db, "INSERT INTO {$table_name} DEFAULT VALUES");

$result = pg_exec($db, "SELECT * FROM ".$table_name);
pg_numrows($result);
pg_numfields($result);
pg_fieldname($result, 0);
pg_fieldsize($result, 0);
pg_fieldtype($result, 0);
pg_fieldprtlen($result, 0);
pg_fieldprtlen($result, null, 0);
pg_fieldisnull($result, 0);
pg_fieldisnull($result, null, 0);
pg_result($result,0,0);
pg_result($result,null,0);

$result = pg_exec($db, "INSERT INTO ".$table_name." VALUES (7777, 'KKK')");
$oid = pg_getlastoid($result);
pg_freeresult($result);
pg_errormessage($db);
$result = pg_exec($db, "UPDATE ".$table_name." SET str = 'QQQ' WHERE str like 'RGD';");
pg_cmdtuples($result);

echo "OK";
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_98old_api";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECTF--
Deprecated: Function pg_numrows() is deprecated since 8.0, use pg_num_rows() instead in %s on line %d

Deprecated: Function pg_numfields() is deprecated since 8.0, use pg_num_fields() instead in %s on line %d

Deprecated: Function pg_fieldname() is deprecated since 8.0, use pg_field_name() instead in %s on line %d

Deprecated: Function pg_fieldsize() is deprecated since 8.0, use pg_field_size() instead in %s on line %d

Deprecated: Function pg_fieldtype() is deprecated since 8.0, use pg_field_type() instead in %s on line %d

Deprecated: Function pg_fieldprtlen() is deprecated since 8.0, use pg_field_prtlen() instead in %s on line %d

Deprecated: Function pg_fieldprtlen() is deprecated since 8.0, use pg_field_prtlen() instead in %s on line %d

Deprecated: pg_fieldprtlen(): Passing null to parameter #2 ($row) of type int is deprecated in %s on line %d

Deprecated: Function pg_fieldisnull() is deprecated since 8.0, use pg_field_is_null() instead in %s on line %d

Deprecated: Function pg_fieldisnull() is deprecated since 8.0, use pg_field_is_null() instead in %s on line %d

Deprecated: pg_fieldisnull(): Passing null to parameter #2 ($row) of type int is deprecated in %s on line %d

Deprecated: Function pg_result() is deprecated since 8.0, use pg_fetch_result() instead in %s on line %d

Deprecated: Function pg_result() is deprecated since 8.0, use pg_fetch_result() instead in %s on line %d

Deprecated: pg_result(): Passing null to parameter #2 ($row) of type int is deprecated in %s on line %d

Deprecated: Function pg_getlastoid() is deprecated since 8.0, use pg_last_oid() instead in %s on line %d

Deprecated: Function pg_freeresult() is deprecated since 8.0, use pg_free_result() instead in %s on line %d

Deprecated: Function pg_errormessage() is deprecated since 8.0, use pg_last_error() instead in %s on line %d

Deprecated: Function pg_cmdtuples() is deprecated since 8.0, use pg_affected_rows() instead in %s on line %d
OK
