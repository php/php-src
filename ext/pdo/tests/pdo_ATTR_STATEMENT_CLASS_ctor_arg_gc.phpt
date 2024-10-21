--TEST--
PDO: Set PDOStatement class with ctor_args that are freed with GC intervention
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

class Bar extends PDO {
    public $statementClass = 'Foo';
    function __construct($dsn, $username, $password, $driver_options = []) {
        $driver_options[PDO::ATTR_ERRMODE] = PDO::ERRMODE_EXCEPTION;
        parent::__construct($dsn, $username, $password, $driver_options);

        $this->setAttribute(PDO::ATTR_STATEMENT_CLASS, [$this->statementClass, [$this]]);
    }
}

$db = PDOTest::factory(Bar::class);

$dummy_query = get_dummy_sql_request();

$stmt = $db->query($dummy_query);
var_dump($stmt instanceof Foo);
var_dump($stmt->queryString === $dummy_query);

?>
--EXPECT--
object(Bar)#1 (1) {
  ["statementClass"]=>
  string(3) "Foo"
}
bool(true)
bool(true)
