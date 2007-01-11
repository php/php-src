--TEST--
oci_define_by_name() for statement re-execution
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__)."/create_table.inc";

$insert_sql = "INSERT INTO ".$schema.$table_name." (id, string) VALUES (1, 'some')";
$s = oci_parse($c, $insert_sql);
var_dump(oci_execute($s));

$insert_sql = "INSERT INTO ".$schema.$table_name." (id, string) VALUES (2, 'thing')";
$s = oci_parse($c, $insert_sql);
var_dump(oci_execute($s));

echo "Test 1 - must do define before execute\n";
$stmt = oci_parse($c, "SELECT string FROM ".$table_name." where id = 1");
oci_execute($stmt);
var_dump(oci_define_by_name($stmt, "STRING", $string));
while (oci_fetch($stmt)) {
	var_dump($string);  // gives NULL
	var_dump(oci_result($stmt, 'STRING'));
}

echo "Test 2 - normal define order\n";
$stmt = oci_parse($c, "SELECT string FROM ".$table_name." where id = 1");
var_dump(oci_define_by_name($stmt, "STRING", $string));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($string);
}

echo "Test 3 - no new define done\n";
$stmt = oci_parse($c, "SELECT string FROM ".$table_name." where id = 2");
oci_execute($stmt);
while (oci_fetch($stmt)) {
	var_dump($string); // not updated with new value
	var_dump(oci_result($stmt, 'STRING'));
}

require dirname(__FILE__)."/drop_table.inc";

echo "Done\n";

?>
--EXPECT--
bool(true)
bool(true)
Test 1 - must do define before execute
bool(true)
NULL
string(4) "some"
Test 2 - normal define order
bool(true)
string(4) "some"
Test 3 - no new define done
string(4) "some"
string(5) "thing"
Done

