--TEST--
Bug #68298 (OCI int overflow)
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

$stmtarray = array(
	"DROP TABLE BUG68298", 
	"CREATE TABLE BUG68298 (COL1 NUMBER(20))"
);

oci8_test_sql_execute($c, $stmtarray);

$s = oci_parse($c, "INSERT INTO BUG68298 VALUES (:INTVALUE)");
$intvalue = 1152921504606846975;
oci_bind_by_name($s, ":INTVALUE", $intvalue, -1, SQLT_INT);
oci_execute($s);

$s = oci_parse($c, "INSERT INTO BUG68298 VALUES (:INTVALUE)");
$intvalue = -1152921504606846975;
oci_bind_by_name($s, ":INTVALUE", $intvalue, -1, SQLT_INT);
oci_execute($s);


$s = oci_parse($c, "SELECT COL1 FROM BUG68298");
oci_execute($s);
oci_fetch_all($s, $r);
var_dump($r);

$stmtarray = array("DROP TABLE BUG68298");
oci8_test_sql_execute($c, $stmtarray);
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(1) {
  ["COL1"]=>
  array(2) {
    [0]=>
    string(19) "1152921504606846975"
    [1]=>
    string(20) "-1152921504606846975"
  }
}
===DONE===
