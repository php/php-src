--TEST--
Bug #45289 (Bogus store_result on PS)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    $link->close();

    $link = mysqli_init();
    if (!($link->real_connect($host, $user, $passwd, $db, $port, $socket)))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    $id = 1;
    if (!($stmt = $link->prepare('SELECT id, label FROM test WHERE id=? LIMIT 1')))
        printf("[002] [%d] %s\n", $link->errno, $link->error);

    if (!$stmt->bind_param('i', $id) || !$stmt->execute())
        printf("[003] [%d] %s\n", $stmt->errno, $stmt->error);

    if ($res = $link->store_result()) {
        printf("[004] Can store result!\n");
    } else {
        printf("[004] [%d] %s\n", $link->errno, $link->error);
    }
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECTF--
[004] [%s
