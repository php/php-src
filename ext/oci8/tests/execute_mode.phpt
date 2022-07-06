--TEST--
oci_execute() and invalid execute mode
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require __DIR__."/connect.inc";

$pc = oci_pconnect($user, $password, $dbase);

$stmt = oci_parse($pc, "select NULL from dual");
oci_execute($stmt, -1);

echo "Done\n";
?>
--EXPECTF--
Warning: oci_execute(): Invalid execute mode given: -1 in %s on line %d
Done
