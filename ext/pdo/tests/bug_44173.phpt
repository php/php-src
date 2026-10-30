--TEST--
PDO Common: Bug #44173 (PDO->query() parameter parsing/checking needs an update)
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec("CREATE TABLE test44173 (x int)");
$db->exec("INSERT INTO test44173 VALUES (1)");


// Bug entry [2] -- 1 is PDO::FETCH_LAZY
try {
    $stmt = $db->query("SELECT * FROM test44173", PDO::FETCH_LAZY, 0, []);
    var_dump($stmt);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}


// Bug entry [3]
try {
    $stmt = $db->query("SELECT * FROM test44173", 'abc');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// Bug entry [4]
try {
    $stmt = $db->query("SELECT * FROM test44173", PDO::FETCH_CLASS, 0, 0, 0);
    var_dump($stmt);
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), \PHP_EOL;
}


// Bug entry [5]
try {
    $stmt = $db->query("SELECT * FROM test44173", PDO::FETCH_INTO);
    var_dump($stmt);
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), \PHP_EOL;
}


// Bug entry [6]
try {
    $stmt = $db->query("SELECT * FROM test44173", PDO::FETCH_COLUMN);
    var_dump($stmt);
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), \PHP_EOL;
}


// Bug entry [7]
try {
    $stmt = $db->query("SELECT * FROM test44173", PDO::FETCH_CLASS);
    var_dump($stmt);
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), \PHP_EOL;
}


?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test44173");
?>
--EXPECT--
PDO::query() expects exactly 2 arguments for the fetch mode provided, 4 given
PDO::query(): Argument #2 ($fetchMode) must be of type ?int, string given
PDO::query() expects at most 4 arguments for the fetch mode provided, 5 given
PDO::query() expects exactly 3 arguments for the fetch mode provided, 2 given
PDO::query() expects exactly 3 arguments for the fetch mode provided, 2 given
PDO::query() expects at least 3 arguments for the fetch mode provided, 2 given
