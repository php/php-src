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

echo "Test 1\n";

$tt = microtime(true);
$c = oci_pconnect('/', 'notemtpy', 'sql1', null, OCI_CRED_EXT);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 2\n";

$tt = microtime(true);
$c = oci_pconnect('notemtpy', 'notemtpy', 'sql1', null, OCI_CRED_EXT);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 3\n";

$tt = microtime(true);
$c = oci_pconnect('notemtpy', '', 'sql1', null, OCI_CRED_EXT);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 4\n";

$tt = microtime(true);
$c = oci_pconnect('a', 'b', 'sql1', null, OCI_SYSDBA+OCI_SYSOPER);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 5\n";

$tt = microtime(true);
$c = oci_pconnect('a', 'b', 'sql1', null, OCI_SYSDBA+OCI_SYSOPER+OCI_CRED_EXT);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 6\n";

$tt = microtime(true);
$c = oci_pconnect('', '', 'sql1', null, OCI_CRED_EXT);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 7\n";

$tt = microtime(true);
$c = oci_pconnect('/', '', 'sql1', null, OCI_CRED_EXT);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 8\n";

$tt = microtime(true);
$c = oci_pconnect('/', null, 'sql1', null, OCI_CRED_EXT);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 9\n";

$tt = microtime(true);
$c = oci_pconnect('/', '', 'sql1', null, OCI_SYSDBA+OCI_CRED_EXT);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);

echo "Test 10\n";

$tt = microtime(true);
$c = oci_pconnect('/', '', 'sql1', null, OCI_SYSOPER+OCI_CRED_EXT);
echo 'elapsed: ' . round(microtime(true) - $tt) . " secs\n";
if (!$c) {
    $m = oci_error();
    var_dump($m);
}
var_dump($c);


?>
--EXPECTF--
Test 1

Warning: oci_pconnect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
elapsed: %c secs
bool(false)
bool(false)
Test 2

Warning: oci_pconnect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
elapsed: %c secs
bool(false)
bool(false)
Test 3

Warning: oci_pconnect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
elapsed: %c secs
bool(false)
bool(false)
Test 4

Warning: oci_pconnect(): OCI_SYSDBA and OCI_SYSOPER cannot be used together in %s on line %d
elapsed: %c secs
bool(false)
bool(false)
Test 5

Warning: oci_pconnect(): OCI_SYSDBA and OCI_SYSOPER cannot be used together in %s on line %d
elapsed: %c secs
bool(false)
bool(false)
Test 6

Warning: oci_pconnect(): OCI_CRED_EXT can only be used with a username of "/" and a NULL password in %s on line %d
elapsed: %c secs
bool(false)
bool(false)
Test 7

Warning: oci_pconnect(): ORA-12154: %s in %s on line %d
elapsed: %c secs
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

Warning: oci_pconnect(): ORA-12154: %s in %s on line %d
elapsed: %c secs
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

Warning: oci_pconnect(): ORA-%d: TNS:%s in %s on line %d
elapsed: %c secs
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

Warning: oci_pconnect(): ORA-%d: TNS:%s in %s on line %d
elapsed: %c secs
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
