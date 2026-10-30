--TEST--
PDO Common: Failed setFetchMode must preserve the prior fetch mode and payload
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
$pdo = PDOTest::factory();

class FetchClass
{
    public int $i;

    public function __construct(public string $marker)
    {
    }
}

$pdo->exec('CREATE TABLE pdo_setfetchmode_atomic (i INT)');
$pdo->exec('INSERT INTO pdo_setfetchmode_atomic (i) VALUES (1)');
$stmt = $pdo->query('SELECT i FROM pdo_setfetchmode_atomic');
$row = new stdClass;
$stmt->setFetchMode(PDO::FETCH_INTO, $row);
try {
    $stmt->setFetchMode(PDO::FETCH_CLASS);
} catch (Throwable $e) {
    echo "into error: ", $e::class, "\n";
}
$r = $stmt->fetch();

echo "into object preserved: ";
var_dump($r === $row);
echo "into value: ", $row->i, "\n";

$stmt = $pdo->query('SELECT i FROM pdo_setfetchmode_atomic');
$stmt->setFetchMode(PDO::FETCH_CLASS, FetchClass::class, ['kept']);
try {
    $stmt->setFetchMode(PDO::FETCH_CLASS);
} catch (Throwable $e) {
    echo "class error: ", $e::class, "\n";
}
$r = $stmt->fetch();

echo "class preserved: ", $r::class, "\n";
echo "constructor argument: ", $r->marker, "\n";
echo "class value: ", $r->i, "\n";
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$pdo = PDOTest::factory();
PDOTest::dropTableIfExists($pdo, 'pdo_setfetchmode_atomic');
?>
--EXPECT--
into error: ArgumentCountError
into object preserved: bool(true)
into value: 1
class error: ArgumentCountError
class preserved: FetchClass
constructor argument: kept
class value: 1
