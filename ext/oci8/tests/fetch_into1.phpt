--TEST--
various ocifetchinto() tests
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";
require __DIR__.'/create_table.inc';

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
var_dump(ocifetchinto($s, $all, OCI_NUM));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_ASSOC));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_RETURN_NULLS));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_RETURN_LOBS));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_NUM+OCI_ASSOC));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_NUM+OCI_ASSOC+OCI_RETURN_NULLS));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_NUM+OCI_ASSOC+OCI_RETURN_NULLS+OCI_RETURN_LOBS));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_RETURN_NULLS+OCI_RETURN_LOBS));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_ASSOC+OCI_RETURN_NULLS+OCI_RETURN_LOBS));
var_dump($all);
var_dump(ocifetchinto($s, $all, OCI_NUM+OCI_RETURN_NULLS+OCI_RETURN_LOBS));
var_dump($all);

require __DIR__.'/drop_table.inc';

echo "Done\n";
?>
--EXPECT--
int(5)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
int(5)
array(2) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
}
int(5)
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
int(5)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
int(5)
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
int(5)
array(10) {
  [0]=>
  string(1) "1"
  ["ID"]=>
  string(1) "1"
  [1]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
  [2]=>
  NULL
  ["BLOB"]=>
  NULL
  [3]=>
  NULL
  ["CLOB"]=>
  NULL
  [4]=>
  NULL
  ["STRING"]=>
  NULL
}
int(5)
array(10) {
  [0]=>
  string(1) "1"
  ["ID"]=>
  string(1) "1"
  [1]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
  [2]=>
  NULL
  ["BLOB"]=>
  NULL
  [3]=>
  NULL
  ["CLOB"]=>
  NULL
  [4]=>
  NULL
  ["STRING"]=>
  NULL
}
int(5)
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
int(5)
array(5) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
  ["BLOB"]=>
  NULL
  ["CLOB"]=>
  NULL
  ["STRING"]=>
  NULL
}
int(5)
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
Done
