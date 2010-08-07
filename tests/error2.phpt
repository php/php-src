--TEST--
Exercise error code for SUCCESS_WITH_INFO
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

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
ORA-24344: success with compilation error
Done
