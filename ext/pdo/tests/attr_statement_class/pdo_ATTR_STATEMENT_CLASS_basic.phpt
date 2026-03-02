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

$table = 'pdo_attr_statement_class_basic';
$db->exec("CREATE TABLE {$table} (id INT, label CHAR(1), PRIMARY KEY(id))");
$db->exec("INSERT INTO {$table} (id, label) VALUES (1, 'a')");
$db->exec("INSERT INTO {$table} (id, label) VALUES (2, 'b')");

$default = $db->getAttribute(PDO::ATTR_STATEMENT_CLASS);
var_dump($default);

// Having a public destructor is allowed
class StatementWithPublicDestructor extends PDOStatement {
    public function __destruct() {
        echo __METHOD__, PHP_EOL;
    }
}

try {
    var_dump($db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['StatementWithPublicDestructor', []]));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
var_dump($db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['StatementWithPublicDestructor']));
$stmt = $db->query("SELECT id, label FROM {$table} ORDER BY id ASC");
unset($stmt);

echo "Class derived from PDOStatement, with private constructor:\n";
class StatementWithPrivateConstructor extends PDOStatement {
    private function __construct($msg) {
        echo __METHOD__, PHP_EOL;
        var_dump($this);
        var_dump($msg);
    }
}
var_dump($db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['StatementWithPrivateConstructor', ['param1']]));
$stmt = $db->query("SELECT id, label FROM {$table} ORDER BY id ASC");
unset($stmt);

echo "Class derived from a child of PDOStatement:\n";
class StatementDerivedFromChild extends StatementWithPrivateConstructor {
    public function fetchAll($fetch_style = 1, ...$fetch_args): array {
        return [];
    }
}

var_dump($db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['StatementDerivedFromChild', ['param1']]));
$stmt = $db->query("SELECT id, label FROM {$table} ORDER BY id ASC");
var_dump($stmt->fetchAll());
unset($stmt);

echo "Reset to default PDOStatement class:\n";
var_dump($db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['PDOStatement']));
$stmt = $db->query("SELECT id, label FROM {$table} ORDER BY id ASC");
var_dump($stmt->fetchAll());
unset($stmt);

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_attr_statement_class_basic");
?>
--EXPECT--
array(1) {
  [0]=>
  string(12) "PDOStatement"
}
bool(true)
bool(true)
StatementWithPublicDestructor::__destruct
Class derived from PDOStatement, with private constructor:
bool(true)
StatementWithPrivateConstructor::__construct
object(StatementWithPrivateConstructor)#2 (1) {
  ["queryString"]=>
  string(68) "SELECT id, label FROM pdo_attr_statement_class_basic ORDER BY id ASC"
}
string(6) "param1"
Class derived from a child of PDOStatement:
bool(true)
StatementWithPrivateConstructor::__construct
object(StatementDerivedFromChild)#2 (1) {
  ["queryString"]=>
  string(68) "SELECT id, label FROM pdo_attr_statement_class_basic ORDER BY id ASC"
}
string(6) "param1"
array(0) {
}
Reset to default PDOStatement class:
bool(true)
array(2) {
  [0]=>
  array(4) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(4) {
    ["id"]=>
    string(1) "2"
    [0]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
    [1]=>
    string(1) "b"
  }
}
