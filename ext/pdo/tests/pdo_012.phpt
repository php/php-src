--TEST--
PDO Common: PDOStatement::setFetchMode
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

$db->exec('CREATE TABLE test012(id int NOT NULL PRIMARY KEY, val VARCHAR(10), grp VARCHAR(10))');
$db->exec('INSERT INTO test012 VALUES(1, \'A\', \'Group1\')');
$db->exec('INSERT INTO test012 VALUES(2, \'B\', \'Group2\')');

$SELECT = 'SELECT val, grp FROM test012';

$stmt = $db->query($SELECT, PDO::FETCH_NUM);
var_dump($stmt->fetchAll());

class Test
{
    public $val;
    public $grp;

    function __construct($name = 'N/A')
    {
        echo __METHOD__ . "($name)\n";
    }
}

unset($stmt);

$stmt = $db->query($SELECT, PDO::FETCH_CLASS, 'Test');
var_dump($stmt->fetchAll());

unset($stmt);

$stmt = $db->query($SELECT, PDO::FETCH_NUM);
$stmt->setFetchMode(PDO::FETCH_CLASS, 'Test', array('Changed'));
var_dump($stmt->fetchAll());

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test012");
?>
--EXPECTF--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "A"
    [1]=>
    string(6) "Group1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "B"
    [1]=>
    string(6) "Group2"
  }
}
Test::__construct(N/A)
Test::__construct(N/A)
array(2) {
  [0]=>
  object(Test)#%d (2) {
    ["val"]=>
    string(1) "A"
    ["grp"]=>
    string(6) "Group1"
  }
  [1]=>
  object(Test)#%d (2) {
    ["val"]=>
    string(1) "B"
    ["grp"]=>
    string(6) "Group2"
  }
}
Test::__construct(Changed)
Test::__construct(Changed)
array(2) {
  [0]=>
  object(Test)#%d (2) {
    ["val"]=>
    string(1) "A"
    ["grp"]=>
    string(6) "Group1"
  }
  [1]=>
  object(Test)#%d (2) {
    ["val"]=>
    string(1) "B"
    ["grp"]=>
    string(6) "Group2"
  }
}
