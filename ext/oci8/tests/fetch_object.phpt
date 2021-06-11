--TEST--
oci_fetch_object()
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table fetch_object_tab",
    "create table fetch_object_tab (\"caseSensitive\" number, secondcol varchar2(20), anothercol char(15))",
    "insert into fetch_object_tab values (123, '1st row col2 string', '1 more text')",
    "insert into fetch_object_tab values (456, '2nd row col2 string', '2 more text')",
    "insert into fetch_object_tab values (789, '3rd row col2 string', '3 more text')",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

if (!($s = oci_parse($c, 'select * from fetch_object_tab'))) {
    die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}

while ($row = oci_fetch_object($s)) {
    var_dump($row);
}

echo "Test 2\n";

if (!($s = oci_parse($c, 'select * from fetch_object_tab'))) {
    die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}

while ($row = oci_fetch_object($s)) {
    echo $row->caseSensitive . "\n";
    echo $row->SECONDCOL . "\n";
    echo $row->ANOTHERCOL . "\n";
}

echo "Test 3\n";

if (!($s = oci_parse($c, 'select * from fetch_object_tab where rownum < 2 order by "caseSensitive"'))) {
    die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}

$row = oci_fetch_object($s);
echo $row->caseSensitive . "\n";
echo $row->CASESENSITIVE . "\n";

// Clean up

$stmtarray = array(
    "drop table fetch_object_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1
object(stdClass)#1 (3) {
  ["caseSensitive"]=>
  string(3) "123"
  ["SECONDCOL"]=>
  string(19) "1st row col2 string"
  ["ANOTHERCOL"]=>
  string(15) "1 more text    "
}
object(stdClass)#2 (3) {
  ["caseSensitive"]=>
  string(3) "456"
  ["SECONDCOL"]=>
  string(19) "2nd row col2 string"
  ["ANOTHERCOL"]=>
  string(15) "2 more text    "
}
object(stdClass)#1 (3) {
  ["caseSensitive"]=>
  string(3) "789"
  ["SECONDCOL"]=>
  string(19) "3rd row col2 string"
  ["ANOTHERCOL"]=>
  string(15) "3 more text    "
}
Test 2
123
1st row col2 string
1 more text    
456
2nd row col2 string
2 more text    
789
3rd row col2 string
3 more text    
Test 3
123

Warning: Undefined property: stdClass::$CASESENSITIVE in %sfetch_object.php on line %d

