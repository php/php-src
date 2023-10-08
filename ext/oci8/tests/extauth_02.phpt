--TEST--
Test External Authentication errors with oci_new_connect
--EXTENSIONS--
oci8
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip feature not available on Windows platforms");
die("skip random CI timeouts caused by Oracle Instant Client, see https://github.com/php/php-src/pull/9524#issuecomment-1244409815");
?>
--INI--
oci8.privileged_connect=1
--FILE--
<?php

error_reporting(E_ALL ^ E_DEPRECATED);

// Run Test

echo "Test 1\n";

$c = oci_new_connect('/', 'notemtpy', 'anything', null, OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 2\n";

$c = oci_new_connect('notemtpy', 'notemtpy', 'anything', null, OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 3\n";

$c = oci_new_connect('notemtpy', '', 'anything', null, OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 4\n";

$c = oci_new_connect('a', 'b', 'c', null, OCI_SYSDBA+OCI_SYSOPER);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 5\n";

$c = oci_new_connect('a', 'b', 'c', null, OCI_SYSDBA+OCI_SYSOPER+OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 6\n";

$c = oci_new_connect('', '', 'anything', null, OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 7\n";

$c = oci_new_connect('/', '', 'anything', null, OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 8\n";

$c = oci_new_connect('/', null, 'anything', null, OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 9\n";

$c = oci_new_connect('/', '', 'd', null, OCI_SYSDBA+OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 10\n";

$c = oci_new_connect('/', '', 'd', null, OCI_SYSOPER+OCI_CRED_EXT);
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);


?>
--EXPECTF--
Test 1

Warning: oci_new_connect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
bool(false)
bool(false)
Test 2

Warning: oci_new_connect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
bool(false)
bool(false)
Test 3

Warning: oci_new_connect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
bool(false)
bool(false)
Test 4

Warning: oci_new_connect(): OCI_SYSDBA and OCI_SYSOPER cannot be used together in %s on line %d
bool(false)
bool(false)
Test 5

Warning: oci_new_connect(): OCI_SYSDBA and OCI_SYSOPER cannot be used together in %s on line %d
bool(false)
bool(false)
Test 6

Warning: oci_new_connect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
bool(false)
bool(false)
Test 7

Warning: oci_new_connect(): ORA-12154: %s in %s on line %d
array(4) {
  ["code"]=>
  int(12154)
  ["message"]=>
  string(%d) "ORA-12154: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}
bool(false)
Test 8

Warning: oci_new_connect(): ORA-12154: %s in %s on line %d
array(4) {
  ["code"]=>
  int(12154)
  ["message"]=>
  string(%d) "ORA-12154: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}
bool(false)
Test 9

Warning: oci_new_connect(): ORA-%d: TNS:%s %s on line %d
array(4) {
  ["code"]=>
  int(%d)
  ["message"]=>
  string(%d) "ORA-%d: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}
bool(false)
Test 10

Warning: oci_new_connect(): ORA-%d: TNS:%s %s on line %d
array(4) {
  ["code"]=>
  int(%d)
  ["message"]=>
  string(%d) "ORA-%d: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}
bool(false)
