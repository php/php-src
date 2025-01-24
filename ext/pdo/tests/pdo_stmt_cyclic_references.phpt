--TEST--
PDO Common: Cyclic PDOStatement child class
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

class Ref {
    public CyclicStatement $stmt;
}

class CyclicStatement extends PDOStatement {
    protected function __construct(public Ref $ref) {}
}

class TestRow {
    public $id;
    public $val;
    public $val2;

    public function __construct(public string $arg) {}
}

$db = PDOTest::factory();
$db->exec('CREATE TABLE pdo_stmt_cyclic_ref(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_stmt_cyclic_ref VALUES(1, 'A', 'AA')");
$db->exec("INSERT INTO pdo_stmt_cyclic_ref VALUES(2, 'B', 'BB')");
$db->exec("INSERT INTO pdo_stmt_cyclic_ref VALUES(3, 'C', 'CC')");

$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['CyclicStatement', [new Ref]]);

echo "Column fetch:\n";
$stmt = $db->query('SELECT id, val2, val FROM pdo_stmt_cyclic_ref');
$stmt->ref->stmt = $stmt;
$stmt->setFetchMode(PDO::FETCH_COLUMN, 2);
foreach($stmt as $obj) {
    var_dump($obj);
}

echo "Class fetch:\n";
$stmt = $db->query('SELECT id, val2, val FROM pdo_stmt_cyclic_ref');
$stmt->ref->stmt = $stmt;
$stmt->setFetchMode(PDO::FETCH_CLASS, 'TestRow', ['Hello world']);
foreach($stmt as $obj) {
    var_dump($obj);
}

echo "Fetch into:\n";
$stmt = $db->query('SELECT id, val2, val FROM pdo_stmt_cyclic_ref');
$stmt->ref->stmt = $stmt;
$stmt->setFetchMode(PDO::FETCH_INTO, new TestRow('I am being fetch into'));
foreach($stmt as $obj) {
    var_dump($obj);
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_stmt_cyclic_ref");
?>
--EXPECTF--
Column fetch:
string(1) "A"
string(1) "B"
string(1) "C"
Class fetch:
object(TestRow)#%d (4) {
  ["id"]=>
  string(1) "1"
  ["val"]=>
  string(1) "A"
  ["val2"]=>
  string(2) "AA"
  ["arg"]=>
  string(11) "Hello world"
}
object(TestRow)#%d (4) {
  ["id"]=>
  string(1) "2"
  ["val"]=>
  string(1) "B"
  ["val2"]=>
  string(2) "BB"
  ["arg"]=>
  string(11) "Hello world"
}
object(TestRow)#%d (4) {
  ["id"]=>
  string(1) "3"
  ["val"]=>
  string(1) "C"
  ["val2"]=>
  string(2) "CC"
  ["arg"]=>
  string(11) "Hello world"
}
Fetch into:
object(TestRow)#4 (4) {
  ["id"]=>
  string(1) "1"
  ["val"]=>
  string(1) "A"
  ["val2"]=>
  string(2) "AA"
  ["arg"]=>
  string(21) "I am being fetch into"
}
object(TestRow)#4 (4) {
  ["id"]=>
  string(1) "2"
  ["val"]=>
  string(1) "B"
  ["val2"]=>
  string(2) "BB"
  ["arg"]=>
  string(21) "I am being fetch into"
}
object(TestRow)#4 (4) {
  ["id"]=>
  string(1) "3"
  ["val"]=>
  string(1) "C"
  ["val2"]=>
  string(2) "CC"
  ["arg"]=>
  string(21) "I am being fetch into"
}
