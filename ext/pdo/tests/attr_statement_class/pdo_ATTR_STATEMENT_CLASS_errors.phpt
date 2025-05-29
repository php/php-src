--TEST--
PDO Common: Set PDOStatement class with PDO::ATTR_STATEMENT_CLASS various error conditions
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
    $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, 'not an array');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

echo "Unknown class variation:\n";
try {
    $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['classname']);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['classname', []]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

echo "Class not derived from PDOStatement:\n";
class NotDerived {
}

try {
    $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['NotDerived']);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['NotDerived', []]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

echo "Class derived from PDOStatement, but with public constructor:\n";
class DerivedWithPublicConstructor extends PDOStatement {
    public function __construct() { }
}

try {
    $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['DerivedWithPublicConstructor']);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['DerivedWithPublicConstructor', []]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
TypeError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_STATEMENT_CLASS value must be of type array, string given
Unknown class variation:
TypeError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_STATEMENT_CLASS class must be a valid class
TypeError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_STATEMENT_CLASS class must be a valid class
Class not derived from PDOStatement:
TypeError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_STATEMENT_CLASS class must be derived from PDOStatement
TypeError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_STATEMENT_CLASS class must be derived from PDOStatement
Class derived from PDOStatement, but with public constructor:
TypeError: PDO::setAttribute(): Argument #2 ($value) User-supplied statement class cannot have a public constructor
TypeError: PDO::setAttribute(): Argument #2 ($value) User-supplied statement class cannot have a public constructor
