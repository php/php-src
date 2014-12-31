--TEST--
oci_field_*() family: error cases
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

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

echo "Test 3\n";
var_dump(oci_field_is_null($c, -1));
var_dump(oci_field_name($c, -1));
var_dump(oci_field_type($c, -1));
var_dump(oci_field_type_raw($c, -1));
var_dump(oci_field_scale($c, -1));
var_dump(oci_field_precision($c, -1));
var_dump(oci_field_size($c, -1));

echo "Test 4\n";
var_dump(oci_field_is_null($s, array()));
var_dump(oci_field_name($s, array()));
var_dump(oci_field_type($s, array()));
var_dump(oci_field_type_raw($s, array()));
var_dump(oci_field_scale($s, array()));
var_dump(oci_field_precision($s, array()));
var_dump(oci_field_size($s, array()));

var_dump(oci_field_size($s));


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
Test 3

Warning: oci_field_is_null(): supplied resource is not a valid oci8 statement resource in %s on line %d
bool(false)

Warning: oci_field_name(): supplied resource is not a valid oci8 statement resource in %s on line %d
bool(false)

Warning: oci_field_type(): supplied resource is not a valid oci8 statement resource in %s on line %d
bool(false)

Warning: oci_field_type_raw(): supplied resource is not a valid oci8 statement resource in %s on line %d
bool(false)

Warning: oci_field_scale(): supplied resource is not a valid oci8 statement resource in %s on line %d
bool(false)

Warning: oci_field_precision(): supplied resource is not a valid oci8 statement resource in %s on line %d
bool(false)

Warning: oci_field_size(): supplied resource is not a valid oci8 statement resource in %s on line %d
bool(false)
Test 4

Warning: oci_field_is_null(): Invalid column index "0" in %s on line %d
bool(false)

Warning: oci_field_name(): Invalid column index "0" in %s on line %d
bool(false)

Warning: oci_field_type(): Invalid column index "0" in %s on line %d
bool(false)

Warning: oci_field_type_raw(): Invalid column index "0" in %s on line %d
bool(false)

Warning: oci_field_scale(): Invalid column index "0" in %s on line %d
bool(false)

Warning: oci_field_precision(): Invalid column index "0" in %s on line %d
bool(false)

Warning: oci_field_size(): Invalid column index "0" in %s on line %d
bool(false)

Warning: oci_field_size() expects exactly 2 parameters, 1 given in %s on line %d
bool(false)
Done
