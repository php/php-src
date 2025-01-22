--TEST--
GH-16703: Memory leak of setFetchMode()
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
class TestStmt extends PDOStatement
{
    public $name;
}

$db = new PDO(
    getenv('PDOTEST_DSN'),
    getenv('PDOTEST_USER') ?: null,
    getenv('PDOTEST_PASS') ?: null,
    [
        PDO::ATTR_CASE => PDO::CASE_LOWER,
        PDO::ATTR_STATEMENT_CLASS => [TestStmt::class],
    ],
);

$db->exec('CREATE TABLE gh16703 (name varchar(255))');
$db->exec("INSERT INTO gh16703 (name) VALUES ('test_name')");

$stmt = $db->query('SELECT name FROM gh16703');
$t = $stmt;
$stmt->setFetchMode(PDO::FETCH_INTO, $stmt);
$stmt->fetch();
echo "done!\n";
?>
--CLEAN--
<?php
if (getenv('PDOTEST_DSN') === 'sqlite::memory:') return;

require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec('DROP TABLE gh16703');
?>
--EXPECT--
done!
