--TEST--
Bug #47243 (Crash on exit with ZTS mode)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Run Test

$s = oci_parse($c, "select cursor(select dummy from dual) from dual");
oci_execute($s);
oci_fetch_all($s, $r);

// No explicit free or close

?>
===DONE===
--EXPECT--
===DONE===
