--TEST--
PDO Common: PDOStatement::setFetchMode() errors
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

$db->exec('CREATE TABLE pdo_set_fetch_mode_error(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_set_fetch_mode_error VALUES(1, 'A', 'AA')");
$db->exec("INSERT INTO pdo_set_fetch_mode_error VALUES(2, 'B', 'BB')");
$db->exec("INSERT INTO pdo_set_fetch_mode_error VALUES(3, 'C', 'CC')");

$stmt = $db->prepare('SELECT id, val, val2 from pdo_set_fetch_mode_error');

foreach (range(PDO::FETCH_KEY_PAIR, 16) as $mode) {
    try {
        echo "Mode: $mode\n";
        var_dump($stmt->setFetchMode($mode));
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
    }
}
foreach (range(PDO::FETCH_KEY_PAIR, 16) as $mode) {
    try {
        echo "Mode: $mode\n";
        var_dump($stmt->setFetchMode($mode|PDO::FETCH_PROPS_LATE));
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
    }
}
try {
    var_dump($stmt->setFetchMode(PDO::FETCH_KEY_PAIR|PDO::FETCH_PROPS_LATE));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    var_dump($stmt->setFetchMode(PDO::FETCH_KEY_PAIR|(PDO::FETCH_SERIALIZE*2)));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    var_dump($stmt->setFetchMode(PDO::FETCH_FUNC));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}


?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_set_fetch_mode_error");
?>
--EXPECT--
Mode: 12
bool(true)
Mode: 13
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) must be a bitmask of PDO::FETCH_* constants
Mode: 14
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) must be a bitmask of PDO::FETCH_* constants
Mode: 15
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) must be a bitmask of PDO::FETCH_* constants
Mode: 16
bool(true)
Mode: 12
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) cannot use PDO::FETCH_CLASSTYPE, PDO::FETCH_PROPS_LATE, or PDO::FETCH_SERIALIZE fetch flags with a fetch mode other than PDO::FETCH_CLASS
Mode: 13
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) cannot use PDO::FETCH_CLASSTYPE, PDO::FETCH_PROPS_LATE, or PDO::FETCH_SERIALIZE fetch flags with a fetch mode other than PDO::FETCH_CLASS
Mode: 14
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) cannot use PDO::FETCH_CLASSTYPE, PDO::FETCH_PROPS_LATE, or PDO::FETCH_SERIALIZE fetch flags with a fetch mode other than PDO::FETCH_CLASS
Mode: 15
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) cannot use PDO::FETCH_CLASSTYPE, PDO::FETCH_PROPS_LATE, or PDO::FETCH_SERIALIZE fetch flags with a fetch mode other than PDO::FETCH_CLASS
Mode: 16
bool(true)
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) cannot use PDO::FETCH_CLASSTYPE, PDO::FETCH_PROPS_LATE, or PDO::FETCH_SERIALIZE fetch flags with a fetch mode other than PDO::FETCH_CLASS
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) must be a bitmask of PDO::FETCH_* constants
ValueError: PDOStatement::setFetchMode(): Argument #1 ($mode) PDO::FETCH_FUNC can only be used with PDOStatement::fetchAll()
