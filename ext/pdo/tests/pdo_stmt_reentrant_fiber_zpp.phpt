--TEST--
PDO Common: PDOStatement operation guard survives a Fiber suspended during argument parsing
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
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR=' . __DIR__ . '/../../pdo/tests/');
}
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

final class SuspendInConstructor
{
    public int $val;

    public function __construct()
    {
        Fiber::suspend();
    }
}

spl_autoload_register(static function (string $class): void {
    Fiber::suspend();
    eval("final class $class {}");
});

$db->exec('CREATE TABLE pdo_reentrant_fiber_zpp (val INT)');
$db->exec('INSERT INTO pdo_reentrant_fiber_zpp VALUES (1)');
$db->exec('INSERT INTO pdo_reentrant_fiber_zpp VALUES (2)');
$statement = $db->query('SELECT val FROM pdo_reentrant_fiber_zpp');

$parseFiber = new Fiber(static function () use ($statement): void {
    try {
        $statement->fetchObject('AutoloadedDuringParsing');
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
});
$parseFiber->start();

$fetchFiber = new Fiber(static fn() => $statement->fetchObject(SuspendInConstructor::class));
$fetchFiber->start();

$parseFiber->resume();

try {
    $statement->setFetchMode(PDO::FETCH_ASSOC);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$fetchFiber->resume();
var_dump($fetchFiber->getReturn() instanceof SuspendInConstructor);
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'pdo_reentrant_fiber_zpp');
?>
--EXPECT--
Error: Cannot perform another operation on this PDOStatement while an operation is in progress
Error: Cannot perform another operation on this PDOStatement while an operation is in progress
bool(true)
