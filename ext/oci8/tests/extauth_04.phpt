--TEST--
Test External Authentication errors on Windows
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
if (substr(PHP_OS, 0, 3) != 'WIN') die("skip this test is for Windows platforms only");
?>
--FILE--
<?php

// Run Test

echo "Test 1\n";

$c = oci_connect('/', '', 'anything', null, OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 2\n";

$c = oci_new_connect('/', '', 'anything', null, OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 3\n";

$c = oci_pconnect('/', '', 'anything', null, OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

?>
--EXPECTF--
Test 1

Warning: oci_connect(): External Authentication is not supported on Windows in %s on line %d
bool(false)
bool(false)
Test 2

Warning: oci_new_connect(): External Authentication is not supported on Windows in %s on line %d
bool(false)
bool(false)
Test 3

Warning: oci_pconnect(): External Authentication is not supported on Windows in %s on line %d
bool(false)
bool(false)
