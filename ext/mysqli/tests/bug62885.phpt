--TEST--
Bug #62885 (mysqli_poll - Segmentation fault)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';
?>
--FILE--
<?php
error_reporting(E_ALL);
$tablica = array();
try {
    $test1 = mysqli_poll($test2, $test3, $tablica, 0);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$test2 = array();
$test2 = array();
try {
    $test1 = mysqli_poll($test2, $test3, $tablica, 0);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "okey";
?>
--EXPECTF--
No stream arrays were passed

Warning: mysqli_poll(): No stream arrays were passed in %s on line %d
okey
