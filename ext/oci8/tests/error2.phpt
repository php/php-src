--TEST--
Exercise error code for SUCCESS_WITH_INFO
--SKIPIF--
<?php 
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
if ($stress_test !== true) die ('skip Slow test not run when $stress_test is FALSE');
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

ini_set('error_reporting', E_ALL);

$s = oci_parse($c, "create or replace procedure myproc as begin bogus end;");
$e = @oci_execute($s);
if (!$e) {
    $es = oci_error($s);
    echo $es['message']."\n";
}

echo "Done\n";

?>
--EXPECTF--
ORA-24344: %s
Done
