--TEST--
PDO Common: PDOStatement rejects reentrant operations while changing parameter bindings
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

function print_exception(Throwable $e): void
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}

final class ReenterOnString
{
    public function __construct(
        private PDOStatement $statement,
        private PDOStatement $otherStatement,
    ) {}

    public function __toString(): string
    {
        try {
            $this->statement->execute([]);
        } catch (Throwable $e) {
            print_exception($e);
        }

        var_dump($this->otherStatement->execute());
        $this->otherStatement->closeCursor();
        return '1';
    }
}

final class ReenterOnDestruct
{
    private WeakReference $statement;

    public function __construct(
        PDOStatement $statement,
        private string $operation,
    ) {
        $this->statement = WeakReference::create($statement);
    }

    public function __destruct()
    {
        $statement = $this->statement->get();
        try {
            if ($this->operation === 'execute') {
                $statement->execute([]);
            } else {
                $statement->bindValue(1, 1, PDO::PARAM_INT);
            }
        } catch (Throwable $e) {
            print_exception($e);
        }
    }
}

$db->exec('CREATE TABLE pdo_reentrant_bind (val INT)');
$db->exec('INSERT INTO pdo_reentrant_bind VALUES (1)');

echo "conversion\n";
$statement = $db->prepare('SELECT val FROM pdo_reentrant_bind WHERE val = ?');
$otherStatement = $db->prepare('SELECT val FROM pdo_reentrant_bind');
$value = new ReenterOnString($statement, $otherStatement);
var_dump($statement->bindParam(1, $value, PDO::PARAM_STR));
var_dump($statement->execute());
var_dump($statement->fetchColumn());
$statement->closeCursor();

echo "replacement\n";
$statement = $db->prepare('SELECT val FROM pdo_reentrant_bind WHERE val = ?');
$statement->bindValue(1, new ReenterOnDestruct($statement, 'execute'), PDO::PARAM_NULL);
var_dump($statement->bindValue(1, 1, PDO::PARAM_INT));
var_dump($statement->execute());
$statement->closeCursor();

echo "teardown\n";
$statement = $db->prepare('SELECT val FROM pdo_reentrant_bind WHERE val = ?');
$statement->bindValue(1, new ReenterOnDestruct($statement, 'bind'), PDO::PARAM_NULL);
var_dump($statement->execute([1]));
$statement->closeCursor();
var_dump($statement->bindValue(1, 1, PDO::PARAM_INT));
var_dump($statement->execute());
var_dump($statement->fetchColumn());
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'pdo_reentrant_bind');
?>
--EXPECT--
conversion
Error: Cannot perform another operation on this PDOStatement while an operation is in progress
bool(true)
bool(true)
bool(true)
string(1) "1"
replacement
Error: Cannot perform another operation on this PDOStatement while an operation is in progress
bool(true)
bool(true)
teardown
Error: Cannot perform another operation on this PDOStatement while an operation is in progress
bool(true)
bool(true)
bool(true)
string(1) "1"
