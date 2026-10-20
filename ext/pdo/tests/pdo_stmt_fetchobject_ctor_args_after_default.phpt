--TEST--
PDO Common: PDOStatement->fetchObject() with $constructorArgs when default CTORs have been set-up
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

class Foo {
    public int $id;

    public function __construct(public string $v) { }
}
class Bar {
    public int $id;
}

$table = 'pdo_stmt_fetchobject_ctor_args_after_default';
$db->exec("CREATE TABLE {$table} (id INT, label CHAR(1), PRIMARY KEY(id))");
$db->exec("INSERT INTO {$table} (id, label) VALUES (1, 'a')");
$db->exec("INSERT INTO {$table} (id, label) VALUES (2, 'b')");
$db->exec("INSERT INTO {$table} (id, label) VALUES (3, 'c')");

$query = "SELECT id FROM {$table} ORDER BY id ASC";
$stmt = $db->prepare($query);
$stmt->setFetchMode(PDO::FETCH_CLASS, Foo::class, ['Hello']);
$stmt->execute();

var_dump($stmt->fetch());
try {
    $obj = $stmt->fetchObject(Bar::class, ['no-args']);
    var_dump($obj);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($stmt->fetch());


?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_stmt_fetchobject_ctor_args_after_default");
?>
--EXPECTF--
object(Foo)#%d (2) {
  ["id"]=>
  int(1)
  ["v"]=>
  string(5) "Hello"
}
ValueError: PDOStatement::fetchObject(): Argument #2 ($constructorArgs) must be empty when class provided in argument #1 ($class) does not have a constructor
object(Foo)#%d (2) {
  ["id"]=>
  int(2)
  ["v"]=>
  string(5) "Hello"
}
