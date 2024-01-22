--TEST--
privileged connect tests
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__."/connect.inc";

oci_connect("", "", "", false, OCI_SYSOPER);
oci_connect("", "", "", false, OCI_SYSDBA);
oci_connect("", "", "", false, -1);

echo "Done\n";
?>
--EXPECTF--
Warning: oci_connect(): Privileged connect is disabled. Enable oci8.privileged_connect to be able to connect as SYSOPER or SYSDBA in %s on line %d

Warning: oci_connect(): Privileged connect is disabled. Enable oci8.privileged_connect to be able to connect as SYSOPER or SYSDBA in %s on line %d

Warning: oci_connect(): Invalid session mode specified (-1) in %s on line %d
Done
