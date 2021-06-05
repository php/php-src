--TEST--
PDO Common: PDOStatement::getColumnMeta
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
/*
 * Note well: meta information is a nightmare to handle portably.
 * it's not really PDOs job.
 * We've not yet defined exactly what makes sense for getColumnMeta,
 * so no tests make any sense to anyone.  When they do, we can enable
 * this test file.
 * TODO: filter out driver dependent components from this common core
 * test file.
 */
?>
--FILE--
<?php

if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
}
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');

/**
 * Enforce basic columns and forbid native_type
 */
function checkColumns(array $meta): void
{
    $expected = [
        'name' => 'string',
        'len' => 'int',
        'precision' => 'int',
    ];
    $forbidden = [
        'native_type'
    ];
    $existing = array_keys($meta);

    foreach ($expected as $key => $type) {
        if (!in_array($key, $existing, true)) {
            throw new Exception('Missing column '.$key);
        }
        if (get_debug_type($meta[$key]) !== $type) {
            throw new Exception('Wrong type for column '.$key.', expected '.$type.' but got '.get_debug_type($meta[$key]));
        }
    }
    foreach ($forbidden as $key) {
        if (in_array($key, $existing, true)) {
            throw new Exception('Forbidden column '.$key);
        }
    }
}

// Insert using question mark placeholders
$stmt = $db->prepare("INSERT INTO test VALUES(?, ?, ?)");
$stmt->execute(array('10', 'Abc', 'zxy'));

// Retrieve column metadata for a result set returned by explicit SELECT
$select = $db->query('SELECT id, val, val2 FROM test');
checkColumns($select->getColumnMeta(0));

try {
    $meta = $select->getColumnMeta(3);
} catch (ValueError $e) {
    printf("%s\n", $e->getMessage());
}
unset($select);

$stmt = $db->prepare("SELECT 42");
if (false !== $stmt->getColumnMeta(0)) {
    throw new Exception('If no columns are present in the statement then the function should return false.');
}

// Retrieve column metadata for a result set returned by a function
$select = $db->query('SELECT COUNT(*) FROM test');
checkColumns($select->getColumnMeta(0));

?>
--EXPECT--
PDOStatement::getColumnMeta(): Argument #1 ($column) must be less than the number of columns
