--TEST--
PDO Common: PDO::FETCH_CLASS using named arguments in constructor array
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

$db->exec('CREATE TABLE pdo_fetch_class_ctor_named(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_class_ctor_named VALUES(1, 'A', 'AA')");
$db->exec("INSERT INTO pdo_fetch_class_ctor_named VALUES(2, 'B', 'BB')");
$db->exec("INSERT INTO pdo_fetch_class_ctor_named VALUES(3, 'C', 'CC')");

$stmt = $db->prepare('SELECT id, val, val2 from pdo_fetch_class_ctor_named');

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
var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'TestBase', ['b' => 'My key is B', 'a' => 'My key is A']));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_class_ctor_named");
?>
--EXPECTF--
Value of $a: My key is A
Value of $b: My key is B
Value of $a: My key is A
Value of $b: My key is B
Value of $a: My key is A
Value of $b: My key is B
array(3) {
  [0]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "1"
    ["val":protected]=>
    string(1) "A"
    ["val2":"TestBase":private]=>
    string(2) "AA"
  }
  [1]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "2"
    ["val":protected]=>
    string(1) "B"
    ["val2":"TestBase":private]=>
    string(2) "BB"
  }
  [2]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "3"
    ["val":protected]=>
    string(1) "C"
    ["val2":"TestBase":private]=>
    string(2) "CC"
  }
}
