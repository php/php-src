--TEST--
PDO Common: PDO::FETCH_FUNC with a simple callback and grouping (PDO::FETCH_FUNC|PDO::FETCH_GROUP)
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

$db->exec('CREATE TABLE pdo_fetch_function_with_grouping(id int NOT NULL PRIMARY KEY, val VARCHAR(10), grp VARCHAR(10))');

$db->exec("INSERT INTO pdo_fetch_function_with_grouping VALUES (1, 'A', 'Group1')");
$db->exec("INSERT INTO pdo_fetch_function_with_grouping VALUES (2, 'B', 'Group1')");
$db->exec("INSERT INTO pdo_fetch_function_with_grouping VALUES (3, 'C', 'Group2')");
$db->exec("INSERT INTO pdo_fetch_function_with_grouping VALUES (4, 'D', 'Group2')");

$selectGroupId = $db->prepare('SELECT grp, id, val FROM pdo_fetch_function_with_grouping');

function associateValWithId(int $id, string $value) {
    echo __FUNCTION__, '(', var_export($id, true), ', ', var_export($value, true) , ")\n";
    return "$id => $value";
}

$selectGroupId->execute();
$result = $selectGroupId->fetchAll(PDO::FETCH_FUNC|PDO::FETCH_GROUP, 'associateValWithId');
var_dump($result);

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_with_grouping");
?>
--EXPECT--
associateValWithId(1, 'A')
associateValWithId(2, 'B')
associateValWithId(3, 'C')
associateValWithId(4, 'D')
array(2) {
  ["Group1"]=>
  array(2) {
    [0]=>
    string(6) "1 => A"
    [1]=>
    string(6) "2 => B"
  }
  ["Group2"]=>
  array(2) {
    [0]=>
    string(6) "3 => C"
    [1]=>
    string(6) "4 => D"
  }
}
