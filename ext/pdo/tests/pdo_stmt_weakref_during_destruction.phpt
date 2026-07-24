--TEST--
PDO Common: PDOStatement invalidates weak references before statement destruction
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if ($dir === false) {
    die('skip no driver');
}
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR=' . __DIR__ . '/../../pdo/tests/');
}
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec('CREATE TABLE pdo_stmt_weakref_dtor (val INT)');

final class StoreStatementWeakReference
{
    public WeakReference $statement;

    public function __destruct()
    {
        $GLOBALS['statement'] = $this->statement->get();
    }
}

$stmt = $db->prepare('SELECT val FROM pdo_stmt_weakref_dtor WHERE val = ?');
$value = new StoreStatementWeakReference();
$value->statement = WeakReference::create($stmt);
$stmt->bindValue(1, $value, PDO::PARAM_NULL);

unset($value, $stmt);

var_dump($statement);
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'pdo_stmt_weakref_dtor');
?>
--EXPECT--
NULL
