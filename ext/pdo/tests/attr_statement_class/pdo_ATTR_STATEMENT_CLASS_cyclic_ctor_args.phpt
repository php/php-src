--TEST--
PDO Common: Set PDOStatement class with PDO::ATTR_STATEMENT_CLASS overall test
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
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

$table = 'pdo_attr_statement_class_cyclic_ctor_args';
$db->exec("CREATE TABLE {$table} (id INT, label CHAR(1), PRIMARY KEY(id))");
$db->exec("INSERT INTO {$table} (id, label) VALUES (1, 'a')");
$db->exec("INSERT INTO {$table} (id, label) VALUES (2, 'b')");

$default = $db->getAttribute(PDO::ATTR_STATEMENT_CLASS);
var_dump($default);

class HoldPdo extends PDOStatement {
    private function __construct(public PDO $v) {
        var_dump($v);
    }
}

var_dump($db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['HoldPdo', [$db]]));
$stmt = $db->query("SELECT id, label FROM {$table} ORDER BY id ASC");
var_dump($stmt);

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_attr_statement_class_cyclic_ctor_args");
?>
--EXPECT--
array(1) {
  [0]=>
  string(12) "PDOStatement"
}
bool(true)
object(PDO)#1 (0) {
}
object(HoldPdo)#2 (2) {
  ["queryString"]=>
  string(79) "SELECT id, label FROM pdo_attr_statement_class_cyclic_ctor_args ORDER BY id ASC"
  ["v"]=>
  object(PDO)#1 (0) {
  }
}
