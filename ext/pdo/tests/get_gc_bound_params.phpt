--TEST--
PDO Common: PDOStatement::get_gc() must report bound params and columns for cycle collection
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

class Tracked {
    public static array $collected = [];

    public function __construct(public string $name, public PDOStatement $stmt) {}

    public function __destruct() {
        self::$collected[] = $this->name;
    }
}

$db = PDOTest::factory();
$db->exec('CREATE TABLE get_gc_bound_params (a INT, b INT)');

$insert = $db->prepare('INSERT INTO get_gc_bound_params VALUES (?, ?)');
$param = new Tracked('param', $insert);
$insert->bindParam(1, $param, PDO::PARAM_INT);
$paramOption = null;
$insert->bindParam(2, $paramOption, PDO::PARAM_INT, 0, new Tracked('param driver option', $insert));

$select = $db->query('SELECT a, b FROM get_gc_bound_params');
$column = new Tracked('column', $select);
$select->bindColumn(1, $column, PDO::PARAM_INT);
$columnOption = null;
$select->bindColumn(2, $columnOption, PDO::PARAM_INT, 0, new Tracked('column driver option', $select));

unset($param, $paramOption, $column, $columnOption, $insert, $select, $db);
var_dump(Tracked::$collected);
gc_collect_cycles();
sort(Tracked::$collected);
var_dump(Tracked::$collected);
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'get_gc_bound_params');
?>
--EXPECT--
array(0) {
}
array(4) {
  [0]=>
  string(6) "column"
  [1]=>
  string(20) "column driver option"
  [2]=>
  string(5) "param"
  [3]=>
  string(19) "param driver option"
}
