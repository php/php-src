--TEST--
Bug #41125 (PDO mysql + quote() + prepare() can result in segfault)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipVersionThanLess(40100);
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

// And now allow the evil to do his work
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
$sql = <<<SQL
    CREATE TABLE IF NOT EXISTS test_41125 (id INT);
    INSERT INTO test_41125 (id) VALUES (1);
    SELECT * FROM test_41125;
    INSERT INTO test_41125 (id) VALUES (2);
    SELECT * FROM test_41125;
SQL;
$stmt = $db->query($sql);
do {
    var_dump($stmt->fetchAll());
} while ($stmt->nextRowset());

print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec("DROP TABLE IF EXISTS test_41125");
?>
--EXPECT--
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
  }
}
array(0) {
}
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    [0]=>
    string(1) "2"
  }
}
done!
