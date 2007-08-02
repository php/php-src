--TEST--
Test functionality disabled in safe mode
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--INI--
safe_mode=On
oci8.privileged_connect=On
--FILE--
<?php

$c = oci_connect("hr", "hrpwd", "//localhost/XE", null, OCI_SYSDBA);

$r = oci_password_change($c, "hr", "hrpwd", "hrpwd");

echo "Done\n";
?>
--EXPECTF--
Warning: oci_connect(): Privileged connect is disabled in Safe Mode in %s on line %d

Warning: oci_password_change(): is disabled in Safe Mode in %s on line %d
Done
