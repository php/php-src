--TEST--
PDO Common: PDOStatement rejects reentrant binding while updating a bound column
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

final class ReenterOnDestruct
{
    private mixed $replacement = null;
    private WeakReference $statement;

    public function __construct(PDOStatement $statement)
    {
        $this->statement = WeakReference::create($statement);
    }

    public function __destruct()
    {
        try {
            $this->statement->get()->bindColumn(1, $this->replacement);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}

$db->exec('CREATE TABLE pdo_reentrant_fetch_bound (id INT, val INT)');
$db->exec('INSERT INTO pdo_reentrant_fetch_bound VALUES (1, 1)');
$db->exec('INSERT INTO pdo_reentrant_fetch_bound VALUES (2, 2)');
$statement = $db->query('SELECT val FROM pdo_reentrant_fetch_bound ORDER BY id');

$value = new ReenterOnDestruct($statement);
$statement->bindColumn(1, $value, PDO::PARAM_INT);
var_dump($statement->fetch(PDO::FETCH_BOUND));
var_dump($value);

$statement->bindColumn(1, $value, PDO::PARAM_INT);
var_dump($statement->fetch(PDO::FETCH_BOUND));
var_dump($value);
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'pdo_reentrant_fetch_bound');
?>
--EXPECT--
Error: Cannot perform another operation on this PDOStatement while an operation is in progress
bool(true)
string(1) "1"
bool(true)
string(1) "2"
