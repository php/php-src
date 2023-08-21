--TEST--
Bug #79375: mysqli_store_result does not report error from lock wait timeout
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
require_once 'skipifconnectfailure.inc';
if (!defined('MYSQLI_STORE_RESULT_COPY_DATA')) die('skip requires mysqlnd');
?>
--FILE--
<?php

require_once 'connect.inc';
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
$mysqli2 = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

$mysqli->query('DROP TABLE IF EXISTS test');
$mysqli->query('CREATE TABLE test (first int) ENGINE = InnoDB');
$mysqli->query('INSERT INTO test VALUES (1),(2),(3),(4),(5),(6),(7),(8),(9)');

function testStmtStoreResult(mysqli $mysqli, string $name) {
    $mysqli->query("SET innodb_lock_wait_timeout = 1");
    $mysqli->query("START TRANSACTION");
    $query = "SELECT first FROM test WHERE first = 1 FOR UPDATE";
    echo "Running query on $name\n";
    $stmt = $mysqli->prepare($query);
    $stmt->execute();
    try {
        $stmt->store_result();
        echo "Got {$stmt->num_rows} for $name\n";
    } catch(mysqli_sql_exception $e) {
        echo $e->getMessage()."\n";
    }
}
function testStmtGetResult(mysqli $mysqli, string $name) {
    $mysqli->query("SET innodb_lock_wait_timeout = 1");
    $mysqli->query("START TRANSACTION");
    $query = "SELECT first FROM test WHERE first = 1 FOR UPDATE";
    echo "Running query on $name\n";
    $stmt = $mysqli->prepare($query);
    $stmt->execute();
    try {
        $res = $stmt->get_result();
        echo "Got {$res->num_rows} for $name\n";
    } catch(mysqli_sql_exception $e) {
        echo $e->getMessage()."\n";
    }
}
function testNormalQuery(mysqli $mysqli, string $name) {
    $mysqli->query("SET innodb_lock_wait_timeout = 1");
    $mysqli->query("START TRANSACTION");
    $query = "SELECT first FROM test WHERE first = 1 FOR UPDATE";
    echo "Running query on $name\n";
    try {
        $res = $mysqli->query($query);
        echo "Got {$res->num_rows} for $name\n";
    } catch(mysqli_sql_exception $e) {
        echo $e->getMessage()."\n";
    }
}
function testStmtUseResult(mysqli $mysqli, string $name) {
    $mysqli->query("SET innodb_lock_wait_timeout = 1");
    $mysqli->query("START TRANSACTION");
    $query = "SELECT first FROM test WHERE first = 1 FOR UPDATE";
    echo "Running query on $name\n";
    $stmt = $mysqli->prepare($query);
    $stmt->execute();
    try {
        $stmt->fetch(); // should throw an error
        $stmt->fetch();
        echo "Got {$stmt->num_rows} for $name\n";
    } catch (mysqli_sql_exception $e) {
        echo $e->getMessage()."\n";
    }
}
function testResultFetchRow(mysqli $mysqli, string $name) {
    $mysqli->query("SET innodb_lock_wait_timeout = 1");
    $mysqli->query("START TRANSACTION");
    $query = "SELECT first FROM test WHERE first = 1 FOR UPDATE";
    echo "Running query on $name\n";
    $res = $mysqli->query($query, MYSQLI_USE_RESULT);
    try {
        $res->fetch_row();
        $res->fetch_row();
        echo "Got {$res->num_rows} for $name\n";
    } catch(mysqli_sql_exception $e) {
        echo $e->getMessage()."\n";
    }
}

testStmtStoreResult($mysqli, 'first connection');
testStmtStoreResult($mysqli2, 'second connection');

$mysqli->close();
$mysqli2->close();

echo "\n";
//  try it again for get_result
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
$mysqli2 = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

testStmtGetResult($mysqli, 'first connection');
testStmtGetResult($mysqli2, 'second connection');

$mysqli->close();
$mysqli2->close();

echo "\n";
//  try it again with unprepared query
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
$mysqli2 = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

testNormalQuery($mysqli, 'first connection');
testNormalQuery($mysqli2, 'second connection');

$mysqli->close();
$mysqli2->close();

echo "\n";
//  try it again with unprepared query
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
$mysqli2 = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

testStmtUseResult($mysqli, 'first connection');
testStmtUseResult($mysqli2, 'second connection');

$mysqli->close();
$mysqli2->close();

echo "\n";
//  try it again using fetch_row on a result object
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
$mysqli2 = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

testResultFetchRow($mysqli, 'first connection');
testResultFetchRow($mysqli2, 'second connection');

$mysqli->close();
$mysqli2->close();

?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECTF--
Running query on first connection
Got %d for first connection
Running query on second connection
Lock wait timeout exceeded; try restarting transaction

Running query on first connection
Got %d for first connection
Running query on second connection
Lock wait timeout exceeded; try restarting transaction

Running query on first connection
Got %d for first connection
Running query on second connection
Lock wait timeout exceeded; try restarting transaction

Running query on first connection
Got %d for first connection
Running query on second connection
Lock wait timeout exceeded; try restarting transaction

Running query on first connection
Got 1 for first connection
Running query on second connection
Lock wait timeout exceeded; try restarting transaction
