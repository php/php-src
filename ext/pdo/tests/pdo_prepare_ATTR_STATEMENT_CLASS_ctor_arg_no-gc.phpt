--TEST--
PDO: Set PDOStatement class with ctor_args that are freed without GC intervention in PDO::prepare()
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

$dummy_query = get_dummy_sql_request();

//$stmt = $db->prepare($dummy_query, [PDO::ATTR_STATEMENT_CLASS => ['Foo', ['param1']] ]);
$stmt = $db->prepare($dummy_query);
var_dump($stmt instanceof Foo);
var_dump($stmt->queryString === $dummy_query);
$r = $stmt->execute();
var_dump($r);

?>
--EXPECT--
string(6) "param1"
bool(true)
bool(true)
bool(true)
