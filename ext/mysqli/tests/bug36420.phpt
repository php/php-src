--TEST--
Bug #36420 (segfault when access result->num_rows after calling result->close())
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

require_once("connect.inc");
$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$result = $mysqli->query('select 1');

$result->close();
echo $result->num_rows;

$mysqli->close();
echo $result->num_rows;

echo "Done\n";
?>
--EXPECTF--
Warning: main(): Couldn't fetch mysqli_result in %s on line %d

Warning: main(): Couldn't fetch mysqli_result in %s on line %d
Done
