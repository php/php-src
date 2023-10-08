--TEST--
Exercise error code for SUCCESS_WITH_INFO
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

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
