--TEST--
Fix GH-18148 pg_copy_from() command position offset when giving explicit \n terminator
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
?>
--FILE--
<?php

include "inc/config.inc";
$table_name = "gh18148";
$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (a integer, b text)");
var_dump(pg_copy_from( $db, $table_name, [ "1\tone\n" ] ));
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS gh18148 cascade");
?>
--EXPECT--
bool(true)
