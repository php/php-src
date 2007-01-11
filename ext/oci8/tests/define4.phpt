--TEST--
oci_define_by_name() on partial number of columns
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__)."/create_table.inc";

$insert_sql = "INSERT INTO ".$schema.$table_name." (value, string) VALUES (1234, 'some')";

if (!($s = oci_parse($c, $insert_sql))) {
        die("oci_parse(insert) failed!\n");
}

if (!oci_execute($s)) {
        die("oci_execute(insert) failed!\n");
}

$stmt = oci_parse($c, "SELECT value, string FROM ".$table_name."");

echo "Test 1\n";
// Only one of the two columns is defined
var_dump(oci_define_by_name($stmt, "STRING", $string));

oci_execute($stmt);

echo "Test 2\n";

while (oci_fetch($stmt)) {
	var_dump(oci_result($stmt, 'VALUE'));
	var_dump($string);
	var_dump(oci_result($stmt, 'STRING'));
	var_dump($string);
	var_dump(oci_result($stmt, 'VALUE'));
	var_dump(oci_result($stmt, 'STRING'));
}

echo "Test 3\n";
var_dump(oci_free_statement($stmt));
var_dump($string);
var_dump(oci_result($stmt, 'STRING'));

require dirname(__FILE__)."/drop_table.inc";

echo "Done\n";

?>
--EXPECTF--
Test 1
bool(true)
Test 2
string(4) "1234"
string(4) "some"
string(4) "some"
string(4) "some"
string(4) "1234"
string(4) "some"
Test 3
bool(true)
string(4) "some"

Warning: oci_result(): %d is not a valid oci8 statement resource in %s on line %d
bool(false)
Done

