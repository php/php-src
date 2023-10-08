--TEST--
Bug #67983: mysqlnd with MYSQLI_OPT_INT_AND_FLOAT_NATIVE fails to interpret bit columns
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require_once 'connect.inc';

$connection = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

mysqli_options($connection, MYSQLI_OPT_INT_AND_FLOAT_NATIVE, true);

mysqli_set_charset($connection, 'utf8');
mysqli_query($connection, 'DROP TABLE IF EXISTS test');
mysqli_query($connection, 'CREATE TABLE test (id BIT(8))');
mysqli_query($connection, 'INSERT INTO test VALUES (0), (1), (42)');

$res = mysqli_query($connection, 'SELECT * FROM test');

while ($result = mysqli_fetch_assoc($res)) {
    var_dump($result['id']);
}

?>
--EXPECT--
int(0)
int(1)
int(42)
