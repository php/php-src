--TEST--
PDO Common: Set PDOStatement class with ctor_args that are freed without GC intervention in PDO::prepare()
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
$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['Foo', ['param1']]);

$table = 'pdo_prepare_attr_statement_class_ctor_arg_no_gc';
$db->exec("CREATE TABLE {$table} (id INT, label CHAR(1), PRIMARY KEY(id))");
$db->exec("INSERT INTO {$table} (id, label) VALUES (1, 'a')");
$db->exec("INSERT INTO {$table} (id, label) VALUES (2, 'b')");

$query = "SELECT id, label FROM {$table} ORDER BY id ASC";

//$stmt = $db->prepare($dummy_query, [PDO::ATTR_STATEMENT_CLASS => ['Foo', ['param1']] ]);
$stmt = $db->prepare($query);

var_dump($stmt instanceof Foo);
var_dump($stmt->queryString === $query);
$r = $stmt->execute();
var_dump($r);
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_prepare_attr_statement_class_ctor_arg_no_gc");
?>
--EXPECT--
string(6) "param1"
bool(true)
bool(true)
bool(true)
