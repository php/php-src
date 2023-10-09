--TEST--
oci_fetch_object() with CLOB and NULL
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

// Initialization

$stmtarray = array(
    "drop table fetch_object_2_tab",
    "create table fetch_object_2_tab (col1 number, col2 CLOB, col3 varchar2(15))",
    "insert into fetch_object_2_tab values (123, '1st row col2 string', '1 more text')",
    "insert into fetch_object_2_tab values (456, '2nd row col2 string', NULL)",
    "insert into fetch_object_2_tab values (789, '3rd row col2 string', '3 more text')",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

if (!($s = oci_parse($c, 'select * from fetch_object_2_tab order by 1'))) {
    die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}

while ($row = oci_fetch_object($s)) {
    var_dump($row);
}

echo "Test 2\n";

if (!($s = oci_parse($c, 'select * from fetch_object_2_tab order by 1'))) {
    die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}

while ($row = oci_fetch_object($s)) {
    echo $row->COL1 . "\n";
    echo $row->COL2->load() . "\n";
    echo $row->COL3 . "\n";
}

// Clean up

$stmtarray = array(
    "drop table fetch_object_2_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1
object(stdClass)#%d (3) {
  ["COL1"]=>
  string(3) "123"
  ["COL2"]=>
  object(OCILob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  ["COL3"]=>
  string(11) "1 more text"
}
object(stdClass)#%d (3) {
  ["COL1"]=>
  string(3) "456"
  ["COL2"]=>
  object(OCILob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  ["COL3"]=>
  NULL
}
object(stdClass)#%d (3) {
  ["COL1"]=>
  string(3) "789"
  ["COL2"]=>
  object(OCILob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  ["COL3"]=>
  string(11) "3 more text"
}
Test 2
123
1st row col2 string
1 more text
456
2nd row col2 string

789
3rd row col2 string
3 more text
