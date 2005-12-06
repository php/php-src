--TEST--
ocifetchinto()
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

/* oci_fetch_all */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
var_dump(ocifetchinto($s, $all));
var_dump($all);

/* oci_fetch_all */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
var_dump(ocifetchinto($s, $all, OCI_NUM+OCI_ASSOC+OCI_RETURN_NULLS+OCI_RETURN_LOBS));
var_dump($all);

require dirname(__FILE__).'/drop_table.inc';
	
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
Done
