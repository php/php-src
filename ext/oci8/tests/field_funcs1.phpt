--TEST--
oci_field_*() family
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__).'/create_table.inc';

$insert_sql = "INSERT INTO ".$schema."".$table_name." (id, value) VALUES (1,1)";

if (!($s = oci_parse($c, $insert_sql))) {
	die("oci_parse(insert) failed!\n");
}

for ($i = 0; $i<3; $i++) {
	if (!oci_execute($s)) {
		die("oci_execute(insert) failed!\n");
	}
}

if (!oci_commit($c)) {
	die("oci_commit() failed!\n");
}

$select_sql = "SELECT * FROM ".$schema."".$table_name."";

if (!($s = oci_parse($c, $select_sql))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

$row = oci_fetch_array($s, OCI_NUM + OCI_RETURN_NULLS + OCI_RETURN_LOBS);
var_dump($row);

var_dump(oci_field_is_null($s, -1));
var_dump(oci_field_name($s, -1));
var_dump(oci_field_type($s, -1));
var_dump(oci_field_type_raw($s, -1));
var_dump(oci_field_scale($s, -1));
var_dump(oci_field_precision($s, -1));
var_dump(oci_field_size($s, -1));

var_dump(oci_field_is_null($s, "none"));
var_dump(oci_field_name($s, "none"));
var_dump(oci_field_type($s, "none"));
var_dump(oci_field_type_raw($s, "none"));
var_dump(oci_field_scale($s, "none"));
var_dump(oci_field_precision($s, "none"));
var_dump(oci_field_size($s, "none"));

var_dump(oci_field_is_null($c, -1));
var_dump(oci_field_name($c, -1));
var_dump(oci_field_type($c, -1));
var_dump(oci_field_type_raw($c, -1));
var_dump(oci_field_scale($c, -1));
var_dump(oci_field_precision($c, -1));
var_dump(oci_field_size($c, -1));

var_dump(oci_field_is_null($s, array()));
var_dump(oci_field_name($s, array()));
var_dump(oci_field_type($s, array()));
var_dump(oci_field_type_raw($s, array()));
var_dump(oci_field_scale($s, array()));
var_dump(oci_field_precision($s, array()));
var_dump(oci_field_size($s, array()));

var_dump(oci_field_size($s));

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
array(5) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  NULL
}

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
