--TEST--
PDO Common: Rejecting ATTR_STATEMENT_CLASS on a persistent connection reports one error
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

putenv("PDOTEST_ATTR=" . serialize([PDO::ATTR_PERSISTENT => true]));
$db = PDOTest::factory();

var_dump($db->setAttribute(PDO::ATTR_STATEMENT_CLASS, [PDOStatement::class]));

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
try {
    $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, [PDOStatement::class]);
} catch (PDOException $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS cannot be used with persistent PDO instances in %s on line %d
bool(false)
PDOException: SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS cannot be used with persistent PDO instances
