--TEST--
binding empty values
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php
	
require dirname(__FILE__).'/connect.inc';

$drop = "DROP table bind_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

$create = "CREATE table bind_test(name VARCHAR(10))";
$statement = oci_parse($c, $create);
oci_execute($statement);


$name = null;
$stmt = oci_parse($c, "UPDATE bind_test SET name=:name");
oci_bind_by_name($stmt, ":name", $name);

$res = oci_execute($stmt);

$name = "";
$stmt = oci_parse($c, "UPDATE bind_test SET name=:name");
oci_bind_by_name($stmt, ":name", $name);

$res = oci_execute($stmt);

$drop = "DROP table bind_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

echo "Done\n";

?>
--EXPECTF--
Done
