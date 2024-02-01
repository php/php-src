--TEST--
Bug #42499 (Multi-statement execution via PDO::exec() makes connection unusable)
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

function bug_42499($db) {
    $db->exec("DROP TABLE IF EXISTS test_42499");
    $db->exec("CREATE TABLE test_42499(id CHAR(1)); INSERT INTO test_42499(id) VALUES ('a')");

    $stmt = $db->query('SELECT id AS _id FROM test_42499');
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    // You must not use exec() to run statements that create a result set!
    $db->exec('SELECT id FROM test_42499');
    // This will bail at you because you have not fetched the SELECT results: this is not a bug!
    $db->exec("INSERT INTO test_42499(id) VALUES ('b')");
}

print "Emulated Prepared Statements...\n";
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 1);
bug_42499($db);

print "Native Prepared Statements...\n";
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 1);
bug_42499($db);

print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec("DROP TABLE IF EXISTS test_42499");
?>
--EXPECTF--
Emulated Prepared Statements...
array(1) {
  [0]=>
  array(1) {
    ["_id"]=>
    string(1) "a"
  }
}

Warning: PDO::exec(): SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute. in %s on line %d
Native Prepared Statements...
array(1) {
  [0]=>
  array(1) {
    ["_id"]=>
    string(1) "a"
  }
}

Warning: PDO::exec(): SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute. in %s on line %d
done!
