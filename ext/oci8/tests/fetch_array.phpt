--TEST--
oci_fetch_array()
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
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

echo "Test 1\n";

$select_sql = "SELECT * FROM ".$schema."".$table_name."";

if (!($s = oci_parse($c, $select_sql))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_array($s)) {
	var_dump($row);
}

echo "Test 2\n";

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_array($s, OCI_NUM)) {
	var_dump($row);
}

echo "Test 3\n";

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_array($s, OCI_ASSOC)) {
	var_dump($row);
}

echo "Test 4\n";

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_array($s, OCI_BOTH)) {
	var_dump($row);
}

echo "Test 5\n";

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_array($s, OCI_RETURN_LOBS)) {
	var_dump($row);
}

echo "Test 6\n";

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_array($s, OCI_RETURN_NULLS)) {
	var_dump($row);
}

echo "Test 7\n";

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_array($s, OCI_NUM+OCI_RETURN_NULLS)) {
	var_dump($row);
}

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";
?>
--EXPECT--
Test 1
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
Test 2
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
Test 3
array(2) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
}
array(2) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
}
array(2) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
}
Test 4
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
Test 5
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
Test 6
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
Test 7
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
