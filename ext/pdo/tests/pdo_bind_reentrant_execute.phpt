--TEST--
PDO: re-executing the statement while a bound value is converted to string
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

class ReExecute
{
    public function __toString(): string
    {
        global $stmt;
        $stmt->execute(['x', 'y']);
        return 'r';
    }
}

$db = PDOTest::factory();
$db->exec('CREATE TABLE test_bind_reentrant (name varchar(10))');
$stmt = $db->prepare('SELECT name FROM test_bind_reentrant WHERE name = ? OR name = ?');

$stmt->execute(['a', new ReExecute()]);
$stmt->bindValue(2, new ReExecute());
echo "Done\n";
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'test_bind_reentrant');
?>
--EXPECT--
Done
