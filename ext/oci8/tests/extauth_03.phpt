--TEST--
Test External Authentication errors with oci_pconnect
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip feature not available on Windows platforms");
?>
--INI--
oci8.privileged_connect=1
--FILE--
<?php

// Run Test

echo "Test 1\n";

$c = oci_pconnect('/', 'notemtpy', 'anything', null, OCI_CRED_EXT);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);

echo "Test 2\n";

$c = oci_pconnect('notemtpy', 'notemtpy', 'anything', null, OCI_CRED_EXT);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);

echo "Test 3\n";

$c = oci_pconnect('notemtpy', '', 'anything', null, OCI_CRED_EXT);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);

echo "Test 4\n";

$c = oci_pconnect('a', 'b', 'c', null, OCI_SYSDBA+OCI_SYSOPER);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);

echo "Test 5\n";

$c = oci_pconnect('a', 'b', 'c', null, OCI_SYSDBA+OCI_SYSOPER+OCI_CRED_EXT);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);

echo "Test 6\n";

$c = oci_pconnect('', '', 'anything', null, OCI_CRED_EXT);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);

echo "Test 7\n";

$c = oci_pconnect('/', '', 'anything', null, OCI_CRED_EXT);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);

echo "Test 8\n";

$c = oci_pconnect('/', null, 'anything', null, OCI_CRED_EXT);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);

echo "Test 9\n";

$c = oci_pconnect('/', '', 'c', null, OCI_SYSDBA+OCI_CRED_EXT);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);

echo "Test 10\n";

$c = oci_pconnect('/', '', 'c', null, OCI_SYSOPER+OCI_CRED_EXT);
if (!$c) {
	$m = oci_error();
	var_dump($m);
}
var_dump($c);


?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1

Warning: oci_pconnect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
bool(false)
bool(false)
Test 2

Warning: oci_pconnect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
bool(false)
bool(false)
Test 3

Warning: oci_pconnect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
bool(false)
bool(false)
Test 4

Warning: oci_pconnect(): OCI_SYSDBA and OCI_SYSOPER cannot be used together in %s on line %d
bool(false)
bool(false)
Test 5

Warning: oci_pconnect(): OCI_SYSDBA and OCI_SYSOPER cannot be used together in %s on line %d
bool(false)
bool(false)
Test 6

Warning: oci_pconnect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
bool(false)
bool(false)
Test 7

Warning: oci_pconnect(): ORA-12154: %s in %s on line %d
array(4) {
  [%u|b%"code"]=>
  int(12154)
  [%u|b%"message"]=>
  %unicode|string%(%d) "ORA-12154: %s"
  [%u|b%"offset"]=>
  int(0)
  [%u|b%"sqltext"]=>
  %unicode|string%(0) ""
}
bool(false)
Test 8

Warning: oci_pconnect(): ORA-12154: %s in %s on line %d
array(4) {
  [%u|b%"code"]=>
  int(12154)
  [%u|b%"message"]=>
  %unicode|string%(%d) "ORA-12154: %s"
  [%u|b%"offset"]=>
  int(0)
  [%u|b%"sqltext"]=>
  %unicode|string%(0) ""
}
bool(false)
Test 9

Warning: oci_pconnect(): ORA-%d: TNS:%s in %s on line %d
array(4) {
  [%u|b%"code"]=>
  int(%d)
  [%u|b%"message"]=>
  %unicode|string%(%d) "ORA-%d: %s"
  [%u|b%"offset"]=>
  int(0)
  [%u|b%"sqltext"]=>
  %unicode|string%(0) ""
}
bool(false)
Test 10

Warning: oci_pconnect(): ORA-%d: TNS:%s in %s on line %d
array(4) {
  [%u|b%"code"]=>
  int(%d)
  [%u|b%"message"]=>
  %unicode|string%(%d) "ORA-%d: %s"
  [%u|b%"offset"]=>
  int(0)
  [%u|b%"sqltext"]=>
  %unicode|string%(0) ""
}
bool(false)
===DONE===
