--TEST--
PDO Common: Bug #46292 (PDO::setFetchMode() shouldn't requires the 2nd arg when using FETCH_CLASSTYPE)
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
$pdoDb = PDOTest::factory();

class myclass {
    public $val;
    public string $group_name;

    public function __construct() {
        printf("%s()\n", __METHOD__);
    }
}

class myclass2 extends myclass { }

$pdoDb->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$pdoDb->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

$pdoDb->query('CREATE TABLE test_46292 (name VARCHAR(20) NOT NULL, group_name VARCHAR(20), val VARCHAR(1))');

$pdoDb->exec("INSERT INTO test_46292 VALUES ('myclass', 'alpha', '1')");
$pdoDb->exec("INSERT INTO test_46292 VALUES ('myclass2', 'beta', '2')");
$pdoDb->exec("INSERT INTO test_46292 VALUES ('myclass', 'alpha', '0')");
$pdoDb->exec("INSERT INTO test_46292 VALUES ('myclass3', 'beta', '0')");
$pdoDb->exec("INSERT INTO test_46292 VALUES ('myclass', 'alpha', '1')");
$pdoDb->exec("INSERT INTO test_46292 VALUES ('myclass2', 'beta', '2')");

$stmt = $pdoDb->prepare("SELECT * FROM test_46292");

var_dump($stmt->setFetchMode(PDO::FETCH_CLASS | PDO::FETCH_CLASSTYPE | PDO::FETCH_GROUP));
$stmt->execute();

var_dump($stmt->fetch());
var_dump($stmt->fetch());
var_dump($stmt->fetchAll());
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test_46292");
?>
--EXPECTF--
bool(true)
myclass::__construct()
object(myclass)#%d (2) {
  ["val"]=>
  string(1) "1"
  ["group_name"]=>
  string(5) "alpha"
}
myclass::__construct()
object(myclass2)#%d (2) {
  ["val"]=>
  string(1) "2"
  ["group_name"]=>
  string(4) "beta"
}
myclass::__construct()
myclass::__construct()
myclass::__construct()
array(2) {
  ["alpha"]=>
  array(2) {
    [0]=>
    object(myclass)#%d (1) {
      ["val"]=>
      string(1) "0"
      ["group_name"]=>
      uninitialized(string)
    }
    [1]=>
    object(myclass)#%d (1) {
      ["val"]=>
      string(1) "1"
      ["group_name"]=>
      uninitialized(string)
    }
  }
  ["beta"]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
      ["val"]=>
      string(1) "0"
    }
    [1]=>
    object(myclass2)#%d (1) {
      ["val"]=>
      string(1) "2"
      ["group_name"]=>
      uninitialized(string)
    }
  }
}
