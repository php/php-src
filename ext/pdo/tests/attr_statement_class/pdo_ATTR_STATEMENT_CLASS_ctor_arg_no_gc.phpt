--TEST--
PDO Common: Set PDOStatement class with ctor_args that are freed without GC intervention
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

class Foo extends PDOStatement {
    private function __construct($v) {
        var_dump($v);
    }
}

$db = PDOTest::factory();

$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('Foo', ['param1']));

$table = 'pdo_attr_statement_class_ctor_arg_no_gc';
$db->exec("CREATE TABLE {$table} (id INT, label CHAR(1), PRIMARY KEY(id))");
$db->exec("INSERT INTO {$table} (id, label) VALUES (1, 'a')");
$db->exec("INSERT INTO {$table} (id, label) VALUES (2, 'b')");
$query = "SELECT id, label FROM {$table} ORDER BY id ASC";
$stmt = $db->query($query);

var_dump($stmt instanceof Foo);
var_dump($stmt->queryString === $query);

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_attr_statement_class_ctor_arg_no_gc");
?>
--EXPECT--
string(6) "param1"
bool(true)
bool(true)
