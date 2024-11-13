--TEST--
PDO MySQL PECL bug #1295 (http://pecl.php.net/bugs/bug.php?id=12925)
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

function bug_pecl_1295($db) {
    $db->exec('DROP TABLE IF EXISTS test_12925');
    $db->exec('CREATE TABLE test_12925(id CHAR(1))');
    $db->exec("INSERT INTO test_12925(id) VALUES ('a')");
    $stmt = $db->prepare("UPDATE test_12925 SET id = 'b'");
    $stmt->execute();
    $stmt = $db->prepare("UPDATE test_12925 SET id = 'c'");
    $stmt->execute();
    $stmt = $db->prepare('SELECT id FROM test_12925');
    $stmt->execute();
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
    $stmt->closeCursor();
}

printf("Emulated...\n");
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
bug_pecl_1295($db);

printf("Native...\n");
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
bug_pecl_1295($db);

print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_12925');
?>
--EXPECT--
Emulated...
array(1) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "c"
  }
}
Native...
array(1) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "c"
  }
}
done!
