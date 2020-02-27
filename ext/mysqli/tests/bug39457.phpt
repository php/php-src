--TEST--
Bug #39457 (Multiple invoked OO connections never close)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $mysql = mysqli_init();
    $mysql->connect($host, $user, $passwd, $db, $port, $socket);

    $mysql->connect($host, $user, $passwd, $db, $port, $socket);

    $mysql->close();
    echo "OK\n";
?>
--EXPECT--
OK
