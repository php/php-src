--TEST--
PDO Common: Setting PDO::FETCH_CLASS with an unknown class
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
$db = PDOTest::factory();

$db->exec('CREATE TABLE pdo_fetch_all_class_error_unknown(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_all_class_error_unknown VALUES(1, 'A', 'AA')");

$stmt = $db->prepare('SELECT id, val, val2 from pdo_fetch_all_class_error_unknown');
$stmt->execute();

try {
    var_dump($stmt->setFetchMode(PDO::FETCH_CLASS, 'Unknown'));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'Unknown'));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_all_class_error_unknown");
?>
--EXPECT--
TypeError: PDOStatement::setFetchMode(): Argument #2 must be a valid class
TypeError: PDOStatement::fetchAll(): Argument #2 must be a valid class
