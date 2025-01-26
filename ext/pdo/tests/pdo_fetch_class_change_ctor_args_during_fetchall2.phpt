--TEST--
PDO Common: PDO::FETCH_CLASS with a constructor that changes the ctor args (Variation fetchAll)
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

// We do not want to abort for this test
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$db->exec('CREATE TABLE pdo_fetch_class_change_ctor(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_class_change_ctor VALUES(1, 'A', 'alpha')");
$db->exec("INSERT INTO pdo_fetch_class_change_ctor VALUES(2, 'B', 'beta')");
$db->exec("INSERT INTO pdo_fetch_class_change_ctor VALUES(3, 'C', 'gamma')");
$db->exec("INSERT INTO pdo_fetch_class_change_ctor VALUES(4, 'D', 'delta')");

$stmt = $db->prepare('SELECT val1, val2 FROM pdo_fetch_class_change_ctor');

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'Test', [$stmt]));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_class_change_ctor");
?>
--EXPECT--
A: alpha
B: beta
C: gamma
D: delta
array(4) {
  [0]=>
  string(8) "A: alpha"
  [1]=>
  string(7) "B: beta"
  [2]=>
  string(8) "C: gamma"
  [3]=>
  string(8) "D: delta"
}
