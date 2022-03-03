--TEST--
Bug #66762 	mysqli@libmysql segfault in mysqli_stmt::bind_result() when link closed
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }

    $read_stmt = $mysqli->prepare("SELECT 1");

    var_dump($read_stmt->bind_result($data));

    unset($mysqli);
    var_dump($read_stmt->bind_result($data));
?>
done!
--EXPECT--
bool(true)
bool(true)
done!
