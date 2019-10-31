--TEST--
Check oci_set_{action,client_identifier,module_name,client_info} error handling
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

error_reporting(E_ALL);
ini_set('display_errors', 'Off');

echo "Test 1\n";

// Generates "ORA-24960: the attribute OCI_ATTR_* is greater than the maximum allowable length of 64"
$s = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

$r = oci_set_action($c, $s);
var_dump($r);
$m = oci_error($c);
echo $m['code'] , "\n";

$r = oci_set_client_identifier($c, $s);
var_dump($r);
$m = oci_error($c);
echo $m['code'] , "\n";

$r = oci_set_module_name($c, $s);
var_dump($r);
$m = oci_error($c);
echo $m['code'] , "\n";

$r = oci_set_client_info($c, $s);
var_dump($r);
$m = oci_error($c);
echo $m['code'] , "\n";

echo "\nTest 2\n";
$s = "x";

$r = oci_set_action($c, $s);
var_dump($r);

$r = oci_set_client_identifier($c, $s);
var_dump($r);

$r = oci_set_module_name($c, $s);
var_dump($r);

$r = oci_set_client_info($c, $s);
var_dump($r);

?>
--EXPECT--
Test 1
bool(false)
24960
bool(false)
24960
bool(false)
24960
bool(false)
24960

Test 2
bool(true)
bool(true)
bool(true)
bool(true)
