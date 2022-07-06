--TEST--
oci_field_*() family: error cases
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table field_funcs1_tab",
    "create table field_funcs1_tab (id number, value number)",
    "insert into field_funcs1_tab (id, value) values (1,1)",
    "insert into field_funcs1_tab (id, value) values (1,1)",
    "insert into field_funcs1_tab (id, value) values (1,1)"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

if (!($s = oci_parse($c, "select * from field_funcs1_tab"))) {
    die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}

$row = oci_fetch_array($s, OCI_NUM + OCI_RETURN_NULLS + OCI_RETURN_LOBS);
var_dump($row);

echo "Test 1\n";
var_dump(oci_field_is_null($s, -1));
var_dump(oci_field_name($s, -1));
var_dump(oci_field_type($s, -1));
var_dump(oci_field_type_raw($s, -1));
var_dump(oci_field_scale($s, -1));
var_dump(oci_field_precision($s, -1));
var_dump(oci_field_size($s, -1));

echo "Test 2\n";
var_dump(oci_field_is_null($s, "none"));
var_dump(oci_field_name($s, "none"));
var_dump(oci_field_type($s, "none"));
var_dump(oci_field_type_raw($s, "none"));
var_dump(oci_field_scale($s, "none"));
var_dump(oci_field_precision($s, "none"));
var_dump(oci_field_size($s, "none"));

// Cleanup

$stmtarray = array(
    "drop table field_funcs1_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECTF--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
Test 1

Warning: oci_field_is_null(): Invalid column index "-1" in %s on line %d
bool(false)

Warning: oci_field_name(): Invalid column index "-1" in %s on line %d
bool(false)

Warning: oci_field_type(): Invalid column index "-1" in %s on line %d
bool(false)

Warning: oci_field_type_raw(): Invalid column index "-1" in %s on line %d
bool(false)

Warning: oci_field_scale(): Invalid column index "-1" in %s on line %d
bool(false)

Warning: oci_field_precision(): Invalid column index "-1" in %s on line %d
bool(false)

Warning: oci_field_size(): Invalid column index "-1" in %s on line %d
bool(false)
Test 2

Warning: oci_field_is_null(): Invalid column name "none" in %s on line %d
bool(false)

Warning: oci_field_name(): Invalid column name "none" in %s on line %d
bool(false)

Warning: oci_field_type(): Invalid column name "none" in %s on line %d
bool(false)

Warning: oci_field_type_raw(): Invalid column name "none" in %s on line %d
bool(false)

Warning: oci_field_scale(): Invalid column name "none" in %s on line %d
bool(false)

Warning: oci_field_precision(): Invalid column name "none" in %s on line %d
bool(false)

Warning: oci_field_size(): Invalid column name "none" in %s on line %d
bool(false)
Done
