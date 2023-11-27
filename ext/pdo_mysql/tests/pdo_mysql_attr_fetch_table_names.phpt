--TEST--
PDO::ATTR_FETCH_TABLE_NAMES
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

    $table = 'pdo_mysql_attr_fetch_table_names';
    MySQLPDOTest::createTestTable($table, $db);

    $db->setAttribute(PDO::ATTR_FETCH_TABLE_NAMES, 1);
    $stmt = $db->query("SELECT label FROM {$table} LIMIT 1");
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
    $stmt->closeCursor();

    $db->setAttribute(PDO::ATTR_FETCH_TABLE_NAMES, 0);
    $stmt = $db->query("SELECT label FROM {$table} LIMIT 1");
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
    $stmt->closeCursor();

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS pdo_mysql_attr_fetch_table_names');
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["pdo_mysql_attr_fetch_table_names.label"]=>
    string(1) "a"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["label"]=>
    string(1) "a"
  }
}
done!
