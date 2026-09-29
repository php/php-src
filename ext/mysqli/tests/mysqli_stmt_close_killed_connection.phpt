--TEST--
Closing a prepared statement after its connection was killed must not leak
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

mysqli_report(MYSQLI_REPORT_OFF);
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

$stmts = [];
for ($i = 0; $i < 20; $i++) {
    $stmts[] = $link->prepare('DO 1');
}
$link->query('KILL ' . $link->thread_id);

// Over TCP, a few COM_STMT_CLOSE writes may be accepted before one fails
do {
    array_pop($stmts);
    usleep(1000);
} while ($stmts && !$link->errno);

echo "done!\n";
?>
--EXPECTF--
Warning: main(): Error occurred while closing statement in %s on line %d
done!
