--TEST--
PDO: bindColumn() must fail for a column name that is not in the result set
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
$db->exec('CREATE TABLE pdo_bindcolumn_unknown_column (name varchar(255))');

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$stmt = $db->query('SELECT name FROM pdo_bindcolumn_unknown_column');
var_dump(@$stmt->bindColumn('nosuchcolumn', $var));

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
try {
    $stmt->bindColumn('nosuchcolumn', $var);
} catch (PDOException $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
if (getenv('PDOTEST_DSN') === 'sqlite::memory:') return;

require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec('DROP TABLE pdo_bindcolumn_unknown_column');
?>
--EXPECT--
bool(false)
PDOException: SQLSTATE[HY000]: General error: Did not find column name 'nosuchcolumn' in the defined columns; it will not be bound
