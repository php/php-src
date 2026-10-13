--TEST--
PDO Common: Constructor option errors report $options as argument #4
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

foreach ([false, true] as $useConnectMethod) {
    foreach ([[PDO::ATTR_ERRMODE => 999], [PDO::ATTR_STATEMENT_CLASS => 'PDOStatement']] as $options) {
        putenv("PDOTEST_ATTR=" . serialize($options));
        try {
            PDOTest::factory(PDO::class, $useConnectMethod);
        } catch (Error $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
    }

    putenv("PDOTEST_ATTR=" . serialize([PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_NUM]));
    $db = PDOTest::factory(PDO::class, $useConnectMethod);
    var_dump($db->getAttribute(PDO::ATTR_DEFAULT_FETCH_MODE) === PDO::FETCH_NUM);
}
?>
--EXPECT--
ValueError: PDO::__construct(): Argument #4 ($options) Error mode must be one of the PDO::ERRMODE_* constants
TypeError: PDO::__construct(): Argument #4 ($options) PDO::ATTR_STATEMENT_CLASS value must be of type array, string given
bool(true)
ValueError: PDO::connect(): Argument #4 ($options) Error mode must be one of the PDO::ERRMODE_* constants
TypeError: PDO::connect(): Argument #4 ($options) PDO::ATTR_STATEMENT_CLASS value must be of type array, string given
bool(true)
