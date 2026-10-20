--TEST--
PDO Common: Invalid Oracle null handling modes do not change the attribute
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
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$pdo = PDOTest::factory();

foreach ([PDO::NULL_NATURAL, PDO::NULL_EMPTY_STRING, PDO::NULL_TO_STRING] as $value) {
    echo "Valid mode: ";
    var_dump($pdo->setAttribute(PDO::ATTR_ORACLE_NULLS, $value));
    var_dump($pdo->getAttribute(PDO::ATTR_ORACLE_NULLS));
}

foreach ([-1, 3, 4, PHP_INT_MAX] as $value) {
    try {
        $pdo->setAttribute(PDO::ATTR_ORACLE_NULLS, $value);
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    echo "Unchanged mode: ";
    var_dump($pdo->getAttribute(PDO::ATTR_ORACLE_NULLS));
}
?>
--EXPECT--
Valid mode: bool(true)
int(0)
Valid mode: bool(true)
int(1)
Valid mode: bool(true)
int(2)
ValueError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_ORACLE_NULLS must be one of the PDO::NULL_* constants
Unchanged mode: int(2)
ValueError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_ORACLE_NULLS must be one of the PDO::NULL_* constants
Unchanged mode: int(2)
ValueError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_ORACLE_NULLS must be one of the PDO::NULL_* constants
Unchanged mode: int(2)
ValueError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_ORACLE_NULLS must be one of the PDO::NULL_* constants
Unchanged mode: int(2)
