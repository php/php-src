--TEST--
Bug #62885 (mysqli_poll - Segmentation fault)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once("connect.inc");
?>
--FILE--
<?php
error_reporting(E_ALL);
$tablica = array();
$test1 = mysqli_poll($test2, $test3, $tablica, 0);

$test2 = array();
$test2 = array();
$test1 = mysqli_poll($test2, $test3, $tablica, 0);
echo "okey";
?>
--EXPECTF--
Warning: mysqli_poll(): No stream arrays were passed in %sbug62885.php on line %d

Warning: mysqli_poll(): No stream arrays were passed in %sbug62885.php on line %d
okey
