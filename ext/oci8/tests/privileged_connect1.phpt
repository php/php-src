--TEST--
privileged connect tests
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
if (getenv('SKIP_ASAN')) die('xleak leaks memory under asan');
?>
--INI--
oci8.privileged_connect=1
--FILE--
<?php

require __DIR__."/connect.inc";

oci_connect("", "", "", false, OCI_SYSOPER);
oci_connect("", "", "", false, OCI_SYSDBA);
oci_connect("", "", "", false, -1);

echo "Done\n";
?>
--EXPECTF--
Warning: oci_connect(): ORA-%d: %s in %s on line %d

Warning: oci_connect(): ORA-%d: %s in %s on line %d

Warning: oci_connect(): Invalid session mode specified (-1) in %s on line %d
Done
