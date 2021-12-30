--TEST--
next_result reports errors from previous calls
--SKIPIF--
<?php
require_once 'skipif.inc';
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require_once __DIR__ . '/connect.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

try {
    $mysqli->query("Syntax Error");
} catch (mysqli_sql_exception $e) {
}
$mysqli->next_result();

print "done!";

?>
--EXPECTF--
done!
