--TEST--
Bug #63398 (Segfault when polling closed link)
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
$link = new mysqli('localhost', 'test', NULL, 'test');

mysqli_close($link);

$read = $error = $reject = array();
$read[] = $error[] = $reject[] = $link;

mysqli_poll($read, $error, $reject, 1);

echo "okey";
?>
--EXPECTF--
Warning: mysqli_poll(): [1] Couldn't fetch mysqli in %sbug63398.php on line %d

Warning: mysqli_poll(): [1] Couldn't fetch mysqli in %sbug63398.php on line %d

Warning: mysqli_poll(): No stream arrays were passed in %sbug63398.php on line %d

Warning: mysqli_poll(): [1] Couldn't fetch mysqli in %sbug63398.php on line %d

Warning: mysqli_poll(): [1] Couldn't fetch mysqli in %sbug63398.php on line %d
okey
