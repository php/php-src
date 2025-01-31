--TEST--
PDO Common: PDOStatement->fetchObject() with $constructorArgs
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

$table = 'pdo_stmt_fetchobject_ctor_args';
$db->exec("CREATE TABLE {$table} (id INT, label CHAR(1), PRIMARY KEY(id))");
$db->exec("INSERT INTO {$table} (id, label) VALUES (1, 'a')");
$db->exec("INSERT INTO {$table} (id, label) VALUES (2, 'b')");
$db->exec("INSERT INTO {$table} (id, label) VALUES (3, 'c')");

$query = "SELECT id FROM {$table} ORDER BY id ASC";
$stmt = $db->prepare($query);

class Foo {
    public int $a;
    public int $id;

    public function __construct($a) {
        $this->a = $a;
    }
}

class Bar {
    public int $id;
}

$stmt->execute();
try {
    $obj = $stmt->fetchObject(Foo::class);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$stmt->execute();
try {
    $obj = $stmt->fetchObject(Foo::class, []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$stmt->execute();
$obj = $stmt->fetchObject(Foo::class, ["a" => 123]);
var_dump($obj);

$stmt->execute();
$obj = $stmt->fetchObject(Bar::class);
var_dump($obj);

$stmt->execute();
$obj = $stmt->fetchObject(Bar::class, []);
var_dump($obj);

try {
    $stmt->execute();
    $obj = $stmt->fetchObject(Bar::class, ["a" => 123]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_stmt_fetchobject_ctor_args");
?>
--EXPECTF--
ArgumentCountError: Too few arguments to function Foo::__construct(), 0 passed and exactly 1 expected
ArgumentCountError: Too few arguments to function Foo::__construct(), 0 passed and exactly 1 expected
object(Foo)#%d (2) {
  ["a"]=>
  int(123)
  ["id"]=>
  int(1)
}
object(Bar)#%d (1) {
  ["id"]=>
  int(1)
}
object(Bar)#%d (1) {
  ["id"]=>
  int(1)
}
ValueError: PDOStatement::fetchObject(): Argument #2 ($constructorArgs) must be empty when class provided in argument #1 ($class) does not have a constructor
