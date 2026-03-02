--TEST--
PDO Common: PECL Bug #5217 (serialize/unserialize safety)
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
try {
    $ser = serialize($db);
    debug_zval_dump($ser);
    $db = unserialize($ser);
    $db->exec('CREATE TABLE test5217 (id int NOT NULL PRIMARY KEY, val VARCHAR(10))');
} catch (Exception $e) {
    echo "Safely caught " . $e->getMessage() . "\n";
}

echo "PHP Didn't crash!\n";
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test5217");
?>
--EXPECT--
Safely caught Serialization of 'PDO' is not allowed
PHP Didn't crash!
