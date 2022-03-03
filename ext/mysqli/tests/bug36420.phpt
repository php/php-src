--TEST--
Bug #36420 (segfault when access result->num_rows after calling result->close())
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

require_once("connect.inc");
$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$result = $mysqli->query('select 1');

$result->close();
try {
    $result->num_rows;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$mysqli->close();
try {
    $result->num_rows;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
mysqli_result object is already closed
mysqli_result object is already closed
Done
