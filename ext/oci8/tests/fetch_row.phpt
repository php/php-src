--TEST--
oci_fetch_row()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table fetch_row_tab",
    "create table fetch_row_tab (id number, value number)",
    "insert into fetch_row_tab (id, value) values (1,1)",
    "insert into fetch_row_tab (id, value) values (1,1)",
    "insert into fetch_row_tab (id, value) values (1,1)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

if (!($s = oci_parse($c, "select * from fetch_row_tab"))) {
    die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_row($s)) {
    var_dump($row);
}

// Cleanup

$stmtarray = array(
    "drop table fetch_row_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
Done
