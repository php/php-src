--TEST--
Bug #79375: mysqli_store_result does not report error from lock wait timeout
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

function createDB(): PDO {
    $db = MySQLPDOTest::factory();
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    return $db;
}

$db = createDB();
$db2 = createDB();
$db->query('DROP TABLE IF EXISTS test');
$db->query('CREATE TABLE test (first int) ENGINE = InnoDB');
$db->query('INSERT INTO test VALUES (1),(2),(3),(4),(5),(6),(7),(8),(9)');

function testNormalQuery(PDO $db, string $name) {
    $db->exec("SET innodb_lock_wait_timeout = 1");
    $db->exec("START TRANSACTION");
    $query = "SELECT first FROM test WHERE first = 1 FOR UPDATE";
    echo "Running query on $name\n";
    try {
        $stmt = $db->query($query);
        echo "Got {$stmt->rowCount()} for $name\n";
    } catch (PDOException $e) {
        echo $e->getMessage()."\n";
    }
}

function testPrepareExecute(PDO $db, string $name) {
    $db->exec("SET innodb_lock_wait_timeout = 1");
    $db->exec("START TRANSACTION");
    $query = "SELECT first FROM test WHERE first = 1 FOR UPDATE";
    echo "Running query on $name\n";
    $stmt = $db->prepare($query);
    try {
        $stmt->execute();
        echo "Got {$stmt->rowCount()} for $name\n";
    } catch (PDOException $e) {
        echo $e->getMessage()."\n";
    }
}

function testUnbuffered(PDO $db, string $name) {
    $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);
    $db->exec("SET innodb_lock_wait_timeout = 1");
    $db->exec("START TRANSACTION");
    $query = "SELECT first FROM test WHERE first = 1 FOR UPDATE";
    echo "Running query on $name\n";
    $stmt = $db->prepare($query);
    $stmt->execute();
    try {
        $rows = $stmt->fetchAll();
        $count = count($rows);
        echo "Got $count for $name\n";
    } catch (PDOException $e) {
        echo $e->getMessage()."\n";
    }
}

testNormalQuery($db, 'first connection');
testNormalQuery($db2, 'second connection');
unset($db);
unset($db2);
echo "\n";

$db = createDB();
$db2 = createDB();
testPrepareExecute($db, 'first connection');
testPrepareExecute($db2, 'second connection');
unset($db);
unset($db2);
echo "\n";

$db = createDB();
$db2 = createDB();
testUnbuffered($db, 'first connection');
testUnbuffered($db2, 'second connection');
unset($db);
unset($db2);
echo "\n";

?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
Running query on first connection
Got 1 for first connection
Running query on second connection
SQLSTATE[HY000]: General error: 1205 Lock wait timeout exceeded; try restarting transaction

Running query on first connection
Got 1 for first connection
Running query on second connection
SQLSTATE[HY000]: General error: 1205 Lock wait timeout exceeded; try restarting transaction

Running query on first connection
Got 1 for first connection
Running query on second connection
SQLSTATE[HY000]: General error: 1205 Lock wait timeout exceeded; try restarting transaction
