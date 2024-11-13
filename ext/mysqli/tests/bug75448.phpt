--TEST--
mysqli_prepare() called on a closed connection should return FALSE (bug #75448)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';
$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);
mysqli_close($link);

try {
    mysqli_prepare($link, 'SELECT VERSION()');
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
mysqli object is already closed
