--TEST--
Bug #74699 (Missing range for FILTER_FLAG_NO_RES_RANGE)
--SKIPIF--
<?php
if (!extension_loaded('filter')) die('skip filter extension not available');
?>
--FILE--
<?php
$return = filter_var("223.255.255.255", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE);
var_dump($return);

$return = filter_var("224.0.0.0", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE);
var_dump($return);

$return = filter_var("224.0.0.2", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE);
var_dump($return);

$return = filter_var("224.0.0.255", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE);
var_dump($return);

$return = filter_var("224.0.1.0", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE);
var_dump($return);

$return = filter_var("224.1.0.0", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE);
var_dump($return);

$return = filter_var("225.0.0.0", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE);
var_dump($return);
?>
--EXPECT--
string(15) "223.255.255.255"
bool(false)
bool(false)
bool(false)
string(9) "224.0.1.0"
string(9) "224.1.0.0"
string(9) "225.0.0.0"
