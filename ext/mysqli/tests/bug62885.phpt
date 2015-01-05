--TEST--
Bug #62885 (mysqli_poll - Segmentation fault)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once("connect.inc");
if (!$IS_MYSQLND) {
    die("skip mysqlnd only test");
}
?>
--FILE--
<?php
error_reporting(E_ALL);
$tablica = array();
$test1 = mysqli_poll($test2, $test3, $tablica, null);

$test2 = array();
$test2 = array();
$test1 = mysqli_poll($test2, $test3, $tablica, null);
echo "okey";
?>
--EXPECTF--
Warning: mysqli_poll(): No stream arrays were passed in %sbug62885.php on line %d

Warning: mysqli_poll(): No stream arrays were passed in %sbug62885.php on line %d
okey
