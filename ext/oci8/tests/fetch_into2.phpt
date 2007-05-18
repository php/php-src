--TEST--
ocifetchinto() & wrong number of params
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__).'/create_table.inc';

$insert_sql = "INSERT INTO ".$schema."".$table_name." (id, value, string) VALUES (1, 1, NULL)";

if (!($s = oci_parse($c, $insert_sql))) {
	die("oci_parse(insert) failed!\n");
}

for ($i = 0; $i<20; $i++) {
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
var_dump(ocifetchinto($s));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_ASSOC, 5));
var_dump($all);
var_dump(ocifetchinto($c, $all, OCI_RETURN_LOBS));
var_dump($all);
var_dump(ocifetchinto($s, $all, 1000000));
var_dump($all);

require dirname(__FILE__).'/drop_table.inc';
	
echo "Done\n";
?>
--EXPECTF--
Warning: ocifetchinto() expects at least 2 parameters, 1 given in %s on line %d
NULL

Notice: Undefined variable: all in %s on line %d
NULL

Warning: ocifetchinto() expects at most 3 parameters, 4 given in %s on line %d
NULL
NULL

Warning: ocifetchinto(): supplied resource is not a valid oci8 statement resource in %s on line %d
bool(false)
NULL
int(5)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
Done
