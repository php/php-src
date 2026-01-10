--TEST--
PDO Common: Setting PDO::FETCH_CLASS with ctor_args when class has no constructor
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

$db->exec('CREATE TABLE pdo_fetch_all_class_ctor_error(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_all_class_ctor_error VALUES(1, 'A', 'AA')");

$stmt = $db->prepare('SELECT id, val, val2 from pdo_fetch_all_class_ctor_error');

class TestBase
{
    public $id;
    protected $val;
    private $val2;
}

$stmt->execute();
try {
    var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'TestBase', [0]));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    var_dump($stmt->setFetchMode(PDO::FETCH_CLASS, 'TestBase', [0]));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_all_class_ctor_error");
?>
--EXPECT--
ValueError: PDOStatement::fetchAll(): Argument #3 must be empty when class provided in argument #2 ($class) does not have a constructor
ValueError: PDOStatement::setFetchMode(): Argument #3 must be empty when class provided in argument #2 ($class) does not have a constructor
