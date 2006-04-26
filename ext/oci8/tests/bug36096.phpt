--TEST--
Bug #36096 (oci_result() returns garbage after oci_fetch() failed)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$sql = "SELECT 'ABC' FROM DUAL WHERE 1<>1";
$stmt = oci_parse($c, $sql);

if(oci_execute($stmt, OCI_COMMIT_ON_SUCCESS)){
	var_dump(oci_fetch($stmt));
	var_dump(oci_result($stmt, 1));
	var_dump(oci_field_name($stmt, 1));
	var_dump(oci_field_type($stmt, 1));
}

echo "Done\n";

?>
--EXPECT--
bool(false)
bool(false)
string(5) "'ABC'"
string(4) "CHAR"
Done
