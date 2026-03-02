--TEST--
PDO Common: PDO::FETCH_FUNC with a simple callback that returns by reference
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

$table = 'pdo_fetch_function_return_by_ref';
$db->exec("CREATE TABLE {$table} (id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10))");
$db->exec("INSERT INTO {$table} VALUES (1, 'A', 'alpha')");
$db->exec("INSERT INTO {$table} VALUES (2, 'B', 'beta')");
$db->exec("INSERT INTO {$table} VALUES (3, 'C', 'gamma')");
$db->exec("INSERT INTO {$table} VALUES (4, 'D', 'delta')");


class Test {
    public array $row = [];

    public function &addRowAndReturnAll(int $id, string $val, string $val2) {
        $this->row = [
            $id,
            $val,
            $val2,
        ];
        return $this->row;
    }
}

$test = new Test();
$selectAll = $db->prepare("SELECT * FROM {$table}");
$selectAll->execute();
$result = $selectAll->fetchAll(PDO::FETCH_FUNC, $test->addRowAndReturnAll(...));
var_dump($result);

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_return_by_ref");
?>
--EXPECT--
array(4) {
  [0]=>
  &array(3) {
    [0]=>
    int(4)
    [1]=>
    string(1) "D"
    [2]=>
    string(5) "delta"
  }
  [1]=>
  &array(3) {
    [0]=>
    int(4)
    [1]=>
    string(1) "D"
    [2]=>
    string(5) "delta"
  }
  [2]=>
  &array(3) {
    [0]=>
    int(4)
    [1]=>
    string(1) "D"
    [2]=>
    string(5) "delta"
  }
  [3]=>
  &array(3) {
    [0]=>
    int(4)
    [1]=>
    string(1) "D"
    [2]=>
    string(5) "delta"
  }
}
