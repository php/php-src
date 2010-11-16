--TEST--
Protect against null bytes in LOB filenames (http://news.php.net/php.internals/50202)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--INI--
display_errors = On
error_reporting = E_WARNING
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Run Test

echo "Test 1: Import\n";

$lob = oci_new_descriptor($c, OCI_D_LOB);
$r = $lob->savefile("/tmp/abc\0def");
var_dump($r);

echo "Test 2: Export\n";

$r = $lob->export("/tmp/abc\0def");
var_dump($r);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1: Import

Warning: OCI-Lob::savefile(): Filename cannot contain null bytes in %snull_byte_1.php on line %d
bool(false)
Test 2: Export

Warning: OCI-Lob::export(): Filename cannot contain null bytes in %snull_byte_1.php on line %d
bool(false)
===DONE===
