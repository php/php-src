--TEST--
PECL Bug #16035 (Crash with Oracle 10.2 connecting with a character set but ORACLE_HOME isn't set)
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die ("skip no oci8 extension"); 
/* Disabled: Fix for PECL Bug #16035 stops a crash if ORACLE_HOME is not set when PHP starts. Using putenv('ORACLE_HOME=""') at runtime will still segfault */
die("skip can't be tested with run-tests.php");
ob_start();
phpinfo(INFO_MODULES);
$phpinfo = ob_get_clean();
$ov = preg_match('/Compile-time ORACLE_HOME/', $phpinfo);
if ($ov !== 1) {
	die ("skip Test only valid when OCI8 is built with an ORACLE_HOME");
}
?>
--FILE--
<?php

putenv('ORACLE_HOME=""');
oci_connect('abc', 'def', 'ghi', 'jkl');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
PHP Warning:  oci_connect(): OCIEnvNlsCreate() failed. There is something wrong with your system - please check that ORACLE_HOME is set and points to the right directory in %s on line %d
===DONE===
