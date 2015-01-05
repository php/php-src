--TEST--
PostgreSQL old api
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$db = pg_connect($conn_str);
$result = pg_exec("SELECT * FROM ".$table_name);
pg_numrows($result);
pg_numfields($result);
pg_fieldname($result, 0);
pg_fieldsize($result, 0);
pg_fieldtype($result, 0);
pg_fieldprtlen($result, 0);
pg_fieldisnull($result, 0);

pg_result($result,0,0);
$result = pg_exec("INSERT INTO ".$table_name." VALUES (7777, 'KKK')");
$oid = pg_getlastoid($result);
pg_freeresult($result);
pg_errormessage();
$result = pg_exec("UPDATE ".$table_name." SET str = 'QQQ' WHERE str like 'RGD';");
pg_cmdtuples($result);



echo "OK";
?>
--EXPECT--
OK
