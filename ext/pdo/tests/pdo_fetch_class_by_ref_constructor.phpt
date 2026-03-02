--TEST--
PDO Common: PDO::FETCH_CLASS with by-ref constructor and arg by value
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

$db->exec('CREATE TABLE pdo_fetch_class_by_ref_ctor(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_class_by_ref_ctor VALUES(1, 'A', 'AA')");
$db->exec("INSERT INTO pdo_fetch_class_by_ref_ctor VALUES(2, 'B', 'BB')");
$db->exec("INSERT INTO pdo_fetch_class_by_ref_ctor VALUES(3, 'C', 'CC')");
$stmt = $db->prepare('SELECT id, val FROM pdo_fetch_class_by_ref_ctor');

class TestByRefCtor
{
    public $id;
    public $val;
    private $str;

    public function __construct(string &$str)
    {
        echo __METHOD__ . "($str, {$this->id})\n";
        $str .= $this->val;
        $this->str = $str;
    }
}

$stmt->execute();
// Use of random_int(10,10) is to defeat SCCP
var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'TestByRefCtor', [str_repeat('a', random_int(10,10))]));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_class_by_ref_ctor");
?>
--EXPECTF--
Warning: TestByRefCtor::__construct(): Argument #1 ($str) must be passed by reference, value given in %s on line %d
TestByRefCtor::__construct(aaaaaaaaaa, 1)

Warning: TestByRefCtor::__construct(): Argument #1 ($str) must be passed by reference, value given in %s on line %d
TestByRefCtor::__construct(aaaaaaaaaa, 2)

Warning: TestByRefCtor::__construct(): Argument #1 ($str) must be passed by reference, value given in %s on line %d
TestByRefCtor::__construct(aaaaaaaaaa, 3)
array(3) {
  [0]=>
  object(TestByRefCtor)#%d (3) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
    ["str":"TestByRefCtor":private]=>
    string(11) "aaaaaaaaaaA"
  }
  [1]=>
  object(TestByRefCtor)#%d (3) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
    ["str":"TestByRefCtor":private]=>
    string(11) "aaaaaaaaaaB"
  }
  [2]=>
  object(TestByRefCtor)#%d (3) {
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
    ["str":"TestByRefCtor":private]=>
    string(11) "aaaaaaaaaaC"
  }
}
