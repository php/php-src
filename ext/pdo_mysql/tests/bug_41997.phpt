--TEST--
PDO MySQL Bug #41997 (stored procedure call returning single rowset blocks future queries)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

$procedure = 'bug_41997_pdo_mysql_p';

$db->exec("CREATE PROCEDURE {$procedure}() BEGIN SELECT 1 AS 'one'; END");

$stmt = $db->query("CALL {$procedure}()");
do {
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
} while ($stmt->nextRowset());
var_dump($stmt->errorInfo());

$stmt = $db->query('SELECT 2 AS "two"');
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
var_dump($stmt->errorInfo());
print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec("DROP PROCEDURE IF EXISTS bug_41997_pdo_mysql_p");
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["one"]=>
    string(1) "1"
  }
}
array(0) {
}
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
array(1) {
  [0]=>
  array(1) {
    ["two"]=>
    string(1) "2"
  }
}
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
done!
