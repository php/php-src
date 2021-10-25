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
--FILE--
<?php

// See http://news.php.net/php.internals/50202
//     http://svn.php.net/viewvc?view=revision&revision=311870

require(__DIR__.'/connect.inc');

// Run Test

echo "Test 1: Import\n";

$lob = oci_new_descriptor($c, OCI_D_LOB);
try {
    $lob->saveFile("/tmp/abc\0def");
} catch (ValueError $e) {
       echo $e->getMessage(), "\n";
}

echo "Test 2: Export\n";

try {
    $lob->export("/tmp/abc\0def");
} catch (ValueError $e) {
       echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Test 1: Import
OCILob::saveFile(): Argument #1 ($filename) must not contain any null bytes
Test 2: Export
OCILob::export(): Argument #1 ($filename) must not contain any null bytes
