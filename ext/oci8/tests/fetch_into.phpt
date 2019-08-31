--TEST--
ocifetchinto()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table fetch_into_tab",
    "create table fetch_into_tab (id number, value number)",
    "insert into fetch_into_tab (id, value) values (1,1)",
    "insert into fetch_into_tab (id, value) values (1,1)",
    "insert into fetch_into_tab (id, value) values (1,1)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

if (!($s = oci_parse($c, "select * from fetch_into_tab"))) {
	die("oci_parse(select) failed!\n");
}

/* ocifetchinto */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
var_dump(ocifetchinto($s, $all));
var_dump($all);

/* ocifetchinto */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
var_dump(ocifetchinto($s, $all, OCI_NUM+OCI_ASSOC+OCI_RETURN_NULLS+OCI_RETURN_LOBS));
var_dump($all);

// Cleanup

$stmtarray = array(
    "drop table fetch_into_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
int(2)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
int(2)
array(4) {
  [0]=>
  string(1) "1"
  ["ID"]=>
  string(1) "1"
  [1]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
}
Done
