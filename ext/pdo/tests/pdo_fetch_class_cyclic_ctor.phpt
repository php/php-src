--TEST--
PDO Common: PDO::FETCH_CLASS with a constructor that changes the ctor args within PDO::fetch()
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
    }
}

$db->exec('CREATE TABLE pdo_fetch_class_cyclic_ctor(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_class_cyclic_ctor VALUES(1, 'A', 'alpha')");
$db->exec("INSERT INTO pdo_fetch_class_cyclic_ctor VALUES(2, 'B', 'beta')");
$db->exec("INSERT INTO pdo_fetch_class_cyclic_ctor VALUES(3, 'C', 'gamma')");
$db->exec("INSERT INTO pdo_fetch_class_cyclic_ctor VALUES(4, 'D', 'delta')");

$args = [];
$args[] = &$args;

$stmt = $db->prepare('SELECT val1, val2 FROM pdo_fetch_class_cyclic_ctor');
$stmt->setFetchMode(PDO::FETCH_CLASS, 'Test', [$args]);

$stmt->execute();
var_dump($stmt->fetch());

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_class_cyclic_ctor");
?>
--EXPECTF--
array(1) {
  [0]=>
  *RECURSION*
}
object(Test)#%d (2) {
  ["val1"]=>
  string(1) "A"
  ["val2"]=>
  string(5) "alpha"
}
