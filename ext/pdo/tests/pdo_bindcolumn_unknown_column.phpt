--TEST--
PDO: bindColumn() must throw for a column name that is not in the result set
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

// The error mode must not affect a ValueError.
foreach ([PDO::ERRMODE_SILENT, PDO::ERRMODE_WARNING, PDO::ERRMODE_EXCEPTION] as $mode) {
    $db->setAttribute(PDO::ATTR_ERRMODE, $mode);
    $stmt = $db->query('SELECT name FROM pdo_bindcolumn_unknown_column');
    try {
        $stmt->bindColumn('nosuchcolumn', $var);
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

// A column that does exist still binds.
$stmt = $db->query('SELECT name FROM pdo_bindcolumn_unknown_column');
var_dump($stmt->bindColumn('name', $var));
?>
--CLEAN--
<?php
if (getenv('PDOTEST_DSN') === 'sqlite::memory:') return;

require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec('DROP TABLE pdo_bindcolumn_unknown_column');
?>
--EXPECT--
ValueError: PDOStatement::bindColumn(): Argument #1 ($column) must refer to a column present in the result set, "nosuchcolumn" given
ValueError: PDOStatement::bindColumn(): Argument #1 ($column) must refer to a column present in the result set, "nosuchcolumn" given
ValueError: PDOStatement::bindColumn(): Argument #1 ($column) must refer to a column present in the result set, "nosuchcolumn" given
bool(true)
