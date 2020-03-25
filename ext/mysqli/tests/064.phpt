--TEST--
NULL binding
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    $stmt = new mysqli_stmt($mysql, "SELECT NULL FROM DUAL");
    $stmt->execute();
    $stmt->bind_result($foo);
    $stmt->fetch();
    $stmt->close();
    $mysql->close();

    var_dump($foo);
?>
--EXPECT--
NULL
