--TEST--
PostgreSQL JIT support
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

$db = pg_connect($conn_str);
var_dump(pg_jit($db));
pg_close($db);
?>
--EXPECTF--
array(2) {
  ["jit_provider"]=>
  %s
  ["jit"]=>
  %s
}
