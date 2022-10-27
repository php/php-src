--TEST--
Bug GH-9841 (mysqli_query throws warning despite using silenced error mode)
--SKIPIF--
<?php
require_once 'skipif.inc';
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require_once 'connect.inc';

mysqli_report(MYSQLI_REPORT_OFF);

$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$mysqli->query("SELECT (
        SELECT 1 AS val
        UNION ALL
        SELECT 2
    ) FROM dual");

$mysqli->query("SELECT (
        SELECT 1 AS val
        UNION ALL
        SELECT 2
    ) FROM dual", MYSQLI_ASYNC);
$mysqli->reap_async_query();

$mysqli->real_query("SELECT (
    SELECT 1 AS val
    UNION ALL
    SELECT 2
) FROM dual");
$result = new mysqli_result($mysqli);

// now make sure the errors are thrown when not using silent mode
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

try {
    $mysqli->query("SELECT (
        SELECT 1 AS val
        UNION ALL
        SELECT 2
    ) FROM dual");
} catch (mysqli_sql_exception $e) {
    echo $e->getMessage()."\n";
}

$mysqli->query("SELECT (
    SELECT 1 AS val
    UNION ALL
    SELECT 2
) FROM dual", MYSQLI_ASYNC);
try {
    $mysqli->reap_async_query();
} catch (mysqli_sql_exception $e) {
    echo $e->getMessage()."\n";
}

$mysqli->real_query("SELECT (
    SELECT 1 AS val
    UNION ALL
    SELECT 2
) FROM dual");
try {
    $result = new mysqli_result($mysqli);
} catch (mysqli_sql_exception $e) {
    echo $e->getMessage()."\n";
}

print "done!";
?>
--EXPECTF--
Subquery returns more than 1 row
Subquery returns more than 1 row
Subquery returns more than 1 row
done!
