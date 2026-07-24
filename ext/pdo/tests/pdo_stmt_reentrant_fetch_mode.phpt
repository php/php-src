--TEST--
PDO Common: PDOStatement rejects reentrant binding while changing the default fetch mode
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

final class FetchTarget
{
    private WeakReference $statement;

    public function __construct(PDOStatement $statement)
    {
        $this->statement = WeakReference::create($statement);
    }

    public function __destruct()
    {
        try {
            $this->statement->get()->bindValue(1, 2, PDO::PARAM_INT);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}

$db->exec('CREATE TABLE pdo_reentrant_fetch_mode (val INT)');
$db->exec('INSERT INTO pdo_reentrant_fetch_mode VALUES (1)');
$statement = $db->prepare('SELECT val FROM pdo_reentrant_fetch_mode WHERE val = ?');
$target = new FetchTarget($statement);
$statement->setFetchMode(PDO::FETCH_INTO, $target);
unset($target);

var_dump($statement->setFetchMode(PDO::FETCH_ASSOC));
var_dump($statement->bindValue(1, 1, PDO::PARAM_INT));
var_dump($statement->execute());
var_dump($statement->fetchColumn());
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'pdo_reentrant_fetch_mode');
?>
--EXPECT--
Error: Cannot perform another operation on this PDOStatement while an operation is in progress
bool(true)
bool(true)
bool(true)
string(1) "1"
