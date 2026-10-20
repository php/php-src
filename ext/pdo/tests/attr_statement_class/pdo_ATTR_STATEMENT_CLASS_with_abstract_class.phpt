--TEST--
PDO Common: Set PDOStatement class with PDO::ATTR_STATEMENT_CLASS that is abstract
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
$table = 'pdo_attr_statement_class_abstract';
$db->exec("CREATE TABLE {$table} (id INT, label CHAR(1), PRIMARY KEY(id))");
$db->exec("INSERT INTO {$table} (id, label) VALUES (1, 'a')");
$db->exec("INSERT INTO {$table} (id, label) VALUES (2, 'b')");

abstract class DerivedButAbstract extends PDOStatement {
    private function __construct() {}
}

try {
    var_dump($db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['DerivedButAbstract', ['DerivedButAbstract']]));
    $stmt = $db->query("SELECT id, label FROM {$table} ORDER BY id ASC");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_attr_statement_class_abstract");
?>
--EXPECT--
bool(true)
Error: Cannot instantiate abstract class DerivedButAbstract
