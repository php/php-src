--TEST--
PDO Common: PDO::FETCH_CLASS using mixed string and int arguments in constructor array
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

$db->exec('CREATE TABLE pdo_fetch_class_ctor_named_and_positional(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_class_ctor_named_and_positional VALUES(1, 'A', 'AA')");
$db->exec("INSERT INTO pdo_fetch_class_ctor_named_and_positional VALUES(2, 'B', 'BB')");
$db->exec("INSERT INTO pdo_fetch_class_ctor_named_and_positional VALUES(3, 'C', 'CC')");

$stmt = $db->prepare('SELECT id, val, val2 from pdo_fetch_class_ctor_named_and_positional');

class TestBase
{
    public $id;
    protected $val;
    private $val2;

    public function __construct(string $a, string $b) {
        echo 'Value of $a: ', $a, PHP_EOL,
            'Value of $b: ', $b, PHP_EOL;
    }
}
$stmt->execute();

try {
    var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'TestBase', ['b' => 'My key is B', 'No key']));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_class_ctor_named_and_positional");
?>
--EXPECT--
Error: Cannot use positional argument after named argument
