--TEST--
Bug #63398 (Segfault when polling closed link)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once("connect.inc");
if (!$IS_MYSQLND) {
    die("skip mysqlnd only test");
}
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require 'connect.inc';
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

mysqli_close($link);

$read = $error = $reject = array();
$read[] = $error[] = $reject[] = $link;

try {
    mysqli_poll($read, $error, $reject, 1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

echo "okey";
?>
--EXPECT--
mysqli object is already closed
okey
