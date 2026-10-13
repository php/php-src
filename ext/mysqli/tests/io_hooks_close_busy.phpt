--TEST--
IO hooks: closing a connection another fiber is suspended in throws instead of freeing it
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$db = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$stmt = $db->prepare('SELECT 1');
$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$scheduler->spawn(function () use ($db) {
    echo "A: query\n";
    $r = $db->query('SELECT SLEEP(0.5) AS s');
    echo "A: ", $r->fetch_assoc()['s'], "\n";
});
$scheduler->spawn(function () use ($db, $stmt) {
    foreach ([fn () => $db->close(), fn () => $stmt->close(), fn () => $db->real_connect()] as $close) {
        try {
            $close();
        } catch (Error $e) {
            echo "B: ", $e->getMessage(), "\n";
        }
    }
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
var_dump($db->close());
?>
--EXPECT--
A: query
B: Concurrent access to a MySQL connection
B: Concurrent access to a MySQL connection
B: Concurrent access to a MySQL connection
A: 0
bool(true)
