--TEST--
Test oci_fetch_all with 0 and -1 skip & maxrows
--EXTENSIONS--
oci8
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table fetch_all5_tab",
    "create table fetch_all5_tab (mycol1 number, mycol2 varchar2(20))",
    "insert into fetch_all5_tab values (1, 'abc')",
    "insert into fetch_all5_tab values (2, 'def')",
    "insert into fetch_all5_tab values (3, 'ghi')"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "select * from fetch_all5_tab order by 1");
oci_execute($s);
$r = oci_fetch_all($s, $res, 0, -1);
var_dump($r);
var_dump($res);

echo "Test 1\n";

$s = oci_parse($c, "select * from fetch_all5_tab order by 1");
oci_execute($s);
$r = oci_fetch_all($s, $res, 0, 0);
var_dump($r);
var_dump($res);

echo "Test 3\n";

$s = oci_parse($c, "select * from fetch_all5_tab order by 1");
oci_execute($s);
$r = oci_fetch_all($s, $res, -1, 0);
var_dump($r);
var_dump($res);

// Clean up

$stmtarray = array(
    "drop table fetch_all5_tab"
);

oci8_test_sql_execute($c, $stmtarray);

oci_close($c);

?>
--EXPECT--
Test 1
int(3)
array(2) {
  ["MYCOL1"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
  }
  ["MYCOL2"]=>
  array(3) {
    [0]=>
    string(3) "abc"
    [1]=>
    string(3) "def"
    [2]=>
    string(3) "ghi"
  }
}
Test 1
int(3)
array(2) {
  ["MYCOL1"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
  }
  ["MYCOL2"]=>
  array(3) {
    [0]=>
    string(3) "abc"
    [1]=>
    string(3) "def"
    [2]=>
    string(3) "ghi"
  }
}
Test 3
int(0)
array(0) {
}
