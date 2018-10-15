--TEST--
Bug #74737: Incorrect ReflectionFunction information for mysqli_get_client_info
--SKIPIF--
<?php
require_once('skipif.inc');
if (!extension_loaded('reflection')) { die("skip"); }
?>
--FILE--
<?php
$client_info = mysqli_get_client_info();
$rf = new ReflectionFunction('mysqli_get_client_info');
echo $rf->getNumberOfParameters();
echo PHP_EOL;
echo $rf->getNumberOfRequiredParameters();
?>
--EXPECT--
0
0
