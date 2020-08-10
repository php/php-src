--TEST--
PDO Common: Bug #44173 (PDO->query() parameter parsing/checking needs an update)
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
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

$db->exec("CREATE TABLE test (x int)");
$db->exec("INSERT INTO test VALUES (1)");


// Bug entry [2] -- 1 is PDO::FETCH_LAZY
$stmt = $db->query("SELECT * FROM test", PDO::FETCH_LAZY, 0, 0);
var_dump($stmt);


// Bug entry [3]
try {
    $stmt = $db->query("SELECT * FROM test", 'abc');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// Bug entry [4]
$stmt = $db->query("SELECT * FROM test", PDO::FETCH_CLASS, 0, 0, 0);
var_dump($stmt);


// Bug entry [5]
$stmt = $db->query("SELECT * FROM test", PDO::FETCH_INTO);
var_dump($stmt);


// Bug entry [6]
$stmt = $db->query("SELECT * FROM test", PDO::FETCH_COLUMN);
var_dump($stmt);


// Bug entry [7]
$stmt = $db->query("SELECT * FROM test", PDO::FETCH_CLASS);
var_dump($stmt);


?>
--EXPECTF--
Warning: PDO::query(): SQLSTATE[HY000]: General error: fetch mode doesn't allow any extra arguments in %s
bool(false)
PDO::query(): Argument #2 ($fetch_mode) must be of type ?int, string given

Warning: PDO::query(): SQLSTATE[HY000]: General error: too many arguments in %s
bool(false)

Warning: PDO::query(): SQLSTATE[HY000]: General error: fetch mode requires the object parameter in %s
bool(false)

Warning: PDO::query(): SQLSTATE[HY000]: General error: fetch mode requires the colno argument in %s
bool(false)

Warning: PDO::query(): SQLSTATE[HY000]: General error: fetch mode requires the classname argument in %s
bool(false)
