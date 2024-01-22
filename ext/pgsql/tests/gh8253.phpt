--TEST--
pg_insert() fails for references
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
?>
--FILE--
<?php
include "inc/config.inc";
$table_name = 'table_gh8253';

function fee(&$a) {}
$a = ["bar" => "testing"];
fee($a["bar"]);

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (bar text);");
pg_insert($db, $table_name, $a);
$res = pg_query($db, "SELECT * FROM {$table_name}");
var_dump(pg_fetch_all($res));
?>
--CLEAN--
<?php
require_once('inc/config.inc');
$table_name = 'table_gh8253';

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["bar"]=>
    string(7) "testing"
  }
}
