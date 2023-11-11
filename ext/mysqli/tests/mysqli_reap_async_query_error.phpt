--TEST--
mysqli_reap_async_query() error reporting
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once "connect.inc";

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$link->query(')', MYSQLI_ASYNC | MYSQLI_USE_RESULT);
$reads = $errors = $rejects = [$link];
mysqli::poll($reads, $errors, $rejects, 1);
$link = $reads[0];

try {
    $rs = $link->reap_async_query();
} catch (mysqli_sql_exception $exception) {
    echo $exception->getMessage() . "\n";
}

print "done!";
?>
--EXPECTF--
You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near ')' at line 1
done!
