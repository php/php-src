--TEST--
Bug GH-14255 (mysqli_fetch_assoc reports error from nested query)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require_once 'connect.inc';

mysqli_report(MYSQLI_REPORT_STRICT|MYSQLI_REPORT_ERROR);

$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$ca = $mysqli->query('SELECT 1 ');
$c = $ca->fetch_assoc();
try {
    $mysqli->query('SELECT non_existent_column');
} catch (Exception $e) {
    echo "Caught exception"."\n";
}
$c = $ca->fetch_assoc();

print "done!";
?>
--EXPECTF--
Caught exception
done!
