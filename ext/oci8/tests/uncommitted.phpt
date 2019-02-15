--TEST--
uncommitted connection
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$stmt = oci_parse($c, "select 1 from dual");
oci_execute($stmt, OCI_DEFAULT);

echo "Done\n";
?>
--EXPECT--
Done
