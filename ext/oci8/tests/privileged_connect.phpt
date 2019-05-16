--TEST--
privileged connect tests
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require __DIR__."/connect.inc";

oci_connect("", "", "", false, OCI_SYSOPER);
oci_connect("", "", "", false, OCI_SYSDBA);
oci_connect("", "", "", false, -1);
oci_connect("", "", "", false, "qwe");

echo "Done\n";
?>
--EXPECTF--
Warning: oci_connect(): Privileged connect is disabled. Enable oci8.privileged_connect to be able to connect as SYSOPER or SYSDBA in %s on line %d

Warning: oci_connect(): Privileged connect is disabled. Enable oci8.privileged_connect to be able to connect as SYSOPER or SYSDBA in %s on line %d

Warning: oci_connect(): Invalid session mode specified (-1) in %s on line %d

Warning: oci_connect() expects parameter 5 to be int%s string given in %s on line %d
Done
