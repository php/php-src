--TEST--
MySQL PDO: PDOStatement->fetchObject() with $constructorArgs
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();

try {
    $query = "SELECT '', NULL, \"\" FROM DUAL";
    $stmt = $db->prepare($query);
    $ok = $stmt->execute();
} catch (PDOException $e) {
    die("skip: Test cannot be run with SQL mode ANSI");
}
if (!$ok)
    die("skip: Test cannot be run with SQL mode ANSI");
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
/** @var PDO $db */
$db = MySQLPDOTest::factory();

$table = 'pdo_mysql_stmt_fetchobject_ctor_args';
MySQLPDOTest::createTestTable($table, $db);

$query = "SELECT id FROM {$table} ORDER BY id ASC LIMIT 1";
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
} catch (ArgumentCountError $exception) {
    echo $exception->getMessage() . "\n";
}

$stmt->execute();
try {
    $obj = $stmt->fetchObject(Foo::class, []);
} catch (ArgumentCountError $exception) {
    echo $exception->getMessage() . "\n";
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
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_stmt_fetchobject_ctor_args');
?>
--EXPECTF--
Too few arguments to function Foo::__construct(), 0 passed and exactly 1 expected
Too few arguments to function Foo::__construct(), 0 passed and exactly 1 expected
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
User-supplied statement does not accept constructor arguments
