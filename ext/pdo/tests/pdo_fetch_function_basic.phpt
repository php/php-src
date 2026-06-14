--TEST--
PDO Common: PDO::FETCH_FUNC with a simple callback
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

$db->exec('CREATE TABLE pdo_fetch_function_basic(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_function_basic VALUES (1, 'A', 'alpha')");
$db->exec("INSERT INTO pdo_fetch_function_basic VALUES (2, 'B', 'beta')");
$db->exec("INSERT INTO pdo_fetch_function_basic VALUES (3, 'C', 'gamma')");
$db->exec("INSERT INTO pdo_fetch_function_basic VALUES (4, 'D', 'delta')");

echo "SELECT id, val1:\n";

function associateValWithId(int $id, string $val): array {
    echo __FUNCTION__, '(', var_export($id, true), ', ', var_export($val, true) , ")\n";
    return [$id=>$val];
}

$selectIdVal = $db->prepare('SELECT id, val1 FROM pdo_fetch_function_basic');
$selectIdVal->execute();
$result = $selectIdVal->fetchAll(PDO::FETCH_FUNC, 'associateValWithId');
var_dump($result);

echo "SELECT *:\n";

function selectAllCallback(int $id, string $val1, string $val2): string {
    echo __FUNCTION__, '(', var_export($id, true),
        ', ', var_export($val1, true),
        ', ', var_export($val2, true) , ")\n";
    return $val1 . $val2;
}

$selectAll = $db->prepare('SELECT * FROM pdo_fetch_function_basic');
$selectAll->execute();
$result = $selectAll->fetchAll(PDO::FETCH_FUNC, 'selectAllCallback');
var_dump($result);

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_basic");
?>
--EXPECT--
SELECT id, val1:
associateValWithId(1, 'A')
associateValWithId(2, 'B')
associateValWithId(3, 'C')
associateValWithId(4, 'D')
array(4) {
  [0]=>
  array(1) {
    [1]=>
    string(1) "A"
  }
  [1]=>
  array(1) {
    [2]=>
    string(1) "B"
  }
  [2]=>
  array(1) {
    [3]=>
    string(1) "C"
  }
  [3]=>
  array(1) {
    [4]=>
    string(1) "D"
  }
}
SELECT *:
selectAllCallback(1, 'A', 'alpha')
selectAllCallback(2, 'B', 'beta')
selectAllCallback(3, 'C', 'gamma')
selectAllCallback(4, 'D', 'delta')
array(4) {
  [0]=>
  string(6) "Aalpha"
  [1]=>
  string(5) "Bbeta"
  [2]=>
  string(6) "Cgamma"
  [3]=>
  string(6) "Ddelta"
}
