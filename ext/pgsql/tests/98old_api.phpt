--TEST--
PostgreSQL old api
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);
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
--EXPECTF--
Deprecated: Function pg_numrows() is deprecated in %s on line %d

Deprecated: Function pg_numfields() is deprecated in %s on line %d

Deprecated: Function pg_fieldname() is deprecated in %s on line %d

Deprecated: Function pg_fieldsize() is deprecated in %s on line %d

Deprecated: Function pg_fieldtype() is deprecated in %s on line %d

Deprecated: Function pg_fieldprtlen() is deprecated in %s on line %d

Deprecated: Function pg_fieldprtlen() is deprecated in %s on line %d

Deprecated: pg_fieldprtlen(): Passing null to parameter #2 ($row) of type int is deprecated in %s on line %d

Deprecated: Function pg_fieldisnull() is deprecated in %s on line %d

Deprecated: Function pg_fieldisnull() is deprecated in %s on line %d

Deprecated: pg_fieldisnull(): Passing null to parameter #2 ($row) of type int is deprecated in %s on line %d

Deprecated: Function pg_result() is deprecated in %s on line %d

Deprecated: Function pg_result() is deprecated in %s on line %d

Deprecated: pg_result(): Passing null to parameter #2 ($row) of type int is deprecated in %s on line %d

Deprecated: Function pg_getlastoid() is deprecated in %s on line %d

Deprecated: Function pg_freeresult() is deprecated in %s on line %d

Deprecated: Function pg_errormessage() is deprecated in %s on line %d

Deprecated: Function pg_cmdtuples() is deprecated in %s on line %d
OK
