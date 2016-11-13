--TEST--
Protect against null bytes in LOB filenames
--SKIPIF--
<?php
if (!extension_loaded('oci8'))
    die ("skip no oci8 extension");
if (PHP_MAJOR_VERSION < 5 || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 4))
    die ("skip Test only valid for PHP 5.4 onwards");
?>
--INI--
display_errors = On
error_reporting = E_WARNING
--FILE--
<?php

// See http://news.php.net/php.internals/50202
//     http://svn.php.net/viewvc?view=revision&revision=311870

require(__DIR__.'/connect.inc');

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

Warning: OCI-Lob::savefile() expects parameter 1 to be a valid path, string given in %snull_byte_1.php on line %d
NULL
Test 2: Export

Warning: OCI-Lob::export() expects parameter 1 to be a valid path, string given in %snull_byte_1.php on line %d
NULL
===DONE===
