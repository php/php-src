--TEST--
PDO Common: Bug #38253 (PDO produces segfault with default fetch mode)
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
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$pdo = PDOTest::factory();

$pdo->exec ("create table test38253 (id integer primary key, n varchar(255))");
$pdo->exec ("INSERT INTO test38253 (id, n) VALUES (1, 'hi')");

try {
    $pdo->setAttribute (PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_CLASS);
    $stmt = $pdo->prepare ("SELECT * FROM test38253");
    $stmt->execute();
    var_dump($stmt->fetchAll());
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    $pdo->setAttribute (PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_FUNC);
    $stmt = $pdo->prepare ("SELECT * FROM test38253");
    $stmt->execute();
    var_dump($stmt->fetchAll());
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    $pdo->setAttribute (PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_INTO);
    $stmt = $pdo->prepare ("SELECT * FROM test38253");
    $stmt->execute();
    var_dump($stmt->fetch());
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test38253");
?>
--EXPECT--
ValueError: PDO::setAttribute(): Argument #2 ($value) cannot set default fetch mode to PDO::FETCH_CLASS without PDO::FETCH_CLASSTYPE
ValueError: PDO::setAttribute(): Argument #2 ($value) PDO::FETCH_FUNC can only be used with PDOStatement::fetchAll()
ValueError: PDO::setAttribute(): Argument #2 ($value) cannot set default fetch mode to PDO::FETCH_USE_DEFAULT, PDO::FETCH_INTO, PDO::FETCH_FUNC
