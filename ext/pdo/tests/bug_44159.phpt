--TEST--
PDO Common: Bug #44159 (Crash: $pdo->setAttribute(PDO::STATEMENT_ATTR_CLASS, NULL))
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
$pdo = PDOTest::factory();
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$attrs = array(PDO::ATTR_STATEMENT_CLASS, PDO::ATTR_STRINGIFY_FETCHES);

foreach ($attrs as $attr) {
    try {
        var_dump($pdo->setAttribute($attr, NULL));
    } catch (\Error $e) {
        echo  get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }
    try {
        var_dump($pdo->setAttribute($attr, 1));
    } catch (\Error $e) {
        echo  get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }
    try {
        var_dump($pdo->setAttribute($attr, 'nonsense'));
    } catch (\Error $e) {
        echo  get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }
}

@unlink(__DIR__."/foo.db");

?>
--EXPECT--
TypeError: PDO::ATTR_STATEMENT_CLASS value must be of type array, null given
TypeError: PDO::ATTR_STATEMENT_CLASS value must be of type array, int given
TypeError: PDO::ATTR_STATEMENT_CLASS value must be of type array, string given
TypeError: Attribute value must be of type bool for selected attribute, null given
bool(true)
TypeError: Attribute value must be of type bool for selected attribute, string given
