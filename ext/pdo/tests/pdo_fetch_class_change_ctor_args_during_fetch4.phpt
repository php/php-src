--TEST--
PDO Common: PDO::FETCH_CLASS with a constructor that changes the ctor args within PDO::fetchAll() (args in fetchAll)
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

class Test {
    public string $val1;
    public string $val2;

    public function __construct(mixed $v) {
        var_dump($v);
        if ($v instanceof PDOStatement) {
            $v->setFetchMode(PDO::FETCH_CLASS, 'Test', [$this->val2]);
        }
    }
}

// TODO Rename test table to pdo_fetch_class_change_ctor_four in PHP-8.4
$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO test VALUES(1, 'A', 'alpha')");
$db->exec("INSERT INTO test VALUES(2, 'B', 'beta')");
$db->exec("INSERT INTO test VALUES(3, 'C', 'gamma')");
$db->exec("INSERT INTO test VALUES(4, 'D', 'delta')");

$stmt = $db->prepare('SELECT val1, val2 FROM test');

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'Test', [$stmt]));

?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(27) "SELECT val1, val2 FROM test"
}
string(5) "alpha"
string(5) "alpha"
string(5) "alpha"
array(4) {
  [0]=>
  object(Test)#%d (2) {
    ["val1"]=>
    string(1) "A"
    ["val2"]=>
    string(5) "alpha"
  }
  [1]=>
  object(Test)#%d (2) {
    ["val1"]=>
    string(1) "B"
    ["val2"]=>
    string(4) "beta"
  }
  [2]=>
  object(Test)#%d (2) {
    ["val1"]=>
    string(1) "C"
    ["val2"]=>
    string(5) "gamma"
  }
  [3]=>
  object(Test)#%d (2) {
    ["val1"]=>
    string(1) "D"
    ["val2"]=>
    string(5) "delta"
  }
}
