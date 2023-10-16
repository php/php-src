--TEST--
PDO Common: PDOStatement iterator
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

$db->exec('CREATE TABLE test013(id int NOT NULL PRIMARY KEY, val VARCHAR(10), grp VARCHAR(10))');
$db->exec("INSERT INTO test013 VALUES(1, 'A', 'Group1')");
$db->exec("INSERT INTO test013 VALUES(2, 'B', 'Group2')");

$SELECT = 'SELECT val, grp FROM test013';

$stmt = $db->prepare($SELECT);

$stmt->execute();
$stmt->setFetchMode(PDO::FETCH_NUM);
foreach ($stmt as $data)
{
    var_dump($data);
}

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

foreach ($db->query($SELECT, PDO::FETCH_CLASS, 'Test') as $data)
{
    var_dump($data);
}

unset($stmt);

$stmt = $db->query($SELECT, PDO::FETCH_CLASS, 'Test', array('WOW'));

foreach($stmt as $data)
{
    var_dump($data);
}
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test013");
?>
--EXPECTF--
array(2) {
  [0]=>
  string(1) "A"
  [1]=>
  string(6) "Group1"
}
array(2) {
  [0]=>
  string(1) "B"
  [1]=>
  string(6) "Group2"
}
Test::__construct(N/A)
object(Test)#%d (2) {
  ["val"]=>
  string(1) "A"
  ["grp"]=>
  string(6) "Group1"
}
Test::__construct(N/A)
object(Test)#%d (2) {
  ["val"]=>
  string(1) "B"
  ["grp"]=>
  string(6) "Group2"
}
Test::__construct(WOW)
object(Test)#%d (2) {
  ["val"]=>
  string(1) "A"
  ["grp"]=>
  string(6) "Group1"
}
Test::__construct(WOW)
object(Test)#%d (2) {
  ["val"]=>
  string(1) "B"
  ["grp"]=>
  string(6) "Group2"
}
