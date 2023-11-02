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

function fee(&$a) {}
$a = ["bar" => "testing"];
fee($a["bar"]);

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS gh8253");
pg_query($db, "CREATE TABLE gh8253 (bar text);");
pg_insert($db, "gh8253", $a);
$res = pg_query($db, "SELECT * FROM gh8253");
var_dump(pg_fetch_all($res));
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["bar"]=>
    string(7) "testing"
  }
}
