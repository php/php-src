--TEST--
PDO Common: PDOStatement operation guard remains held across Fiber suspension
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

final class SuspendOnString
{
    public function __toString(): string
    {
        Fiber::suspend('bind');
        return '1';
    }
}

$db->exec('CREATE TABLE pdo_reentrant_fiber (val INT)');
$db->exec('INSERT INTO pdo_reentrant_fiber VALUES (1)');
$statement = $db->prepare('SELECT val FROM pdo_reentrant_fiber WHERE val = ?');
$statement->execute([1]);

set_error_handler(static function (): bool {
    static $suspended = false;
    if (!$suspended) {
        $suspended = true;
        Fiber::suspend('handler');
    }
    return true;
});

$fetchFiber = new Fiber(static function () use ($statement): void {
    $statement->fetch(PDO::FETCH_CLASS | PDO::FETCH_SERIALIZE);
});
var_dump($fetchFiber->start());

$bindFiber = new Fiber(static function () use ($statement): void {
    try {
        $statement->bindValue(1, new SuspendOnString(), PDO::PARAM_STR);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
});
$bindFiber->start();

var_dump($fetchFiber->resume());
var_dump($fetchFiber->isTerminated());
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'pdo_reentrant_fiber');
?>
--EXPECT--
string(7) "handler"
Error: Cannot perform another operation on this PDOStatement while an operation is in progress
NULL
bool(true)
