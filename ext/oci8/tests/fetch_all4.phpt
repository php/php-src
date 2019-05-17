--TEST--
Test oci_fetch_* array overwriting when query returns no rows
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table fetch_all4_tab",
	"create table fetch_all4_tab (mycol1 number, mycol2 varchar2(20))",
	"insert into fetch_all4_tab values (1, 'abc')"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "select * from fetch_all4_tab where 1 = 0");
oci_execute($s);
$res = array(1,2,3);  // this array is replaced as a result of the query
$r = oci_fetch_all($s, $res);
var_dump($r);
var_dump($res);

echo "Test 2\n";

$s = oci_parse($c, "select * from fetch_all4_tab where 1 = 0");
oci_execute($s);
$row = array(1,2,3);  // this array is replaced as a result of the query
$row = oci_fetch_array($s);
var_dump($row);

// Clean up

$stmtarray = array(
	"drop table fetch_all4_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1
int(0)
array(2) {
  ["MYCOL1"]=>
  array(0) {
  }
  ["MYCOL2"]=>
  array(0) {
  }
}
Test 2
bool(false)
===DONE===
