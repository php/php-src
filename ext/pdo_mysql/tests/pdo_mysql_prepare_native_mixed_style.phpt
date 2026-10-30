--TEST--
MySQL PDO->prepare(), native PS, mixed, wired style
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

    $table = 'pdo_mysql_prepare_native_mixed_style';
    MySQLPDOTest::createTestTable($table, $db);

    $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
    if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
        printf("[002] Unable to turn off emulated prepared statements\n");

    $stmt = $db->query("DELETE FROM {$table}");
    $stmt = $db->prepare("INSERT INTO {$table}(id, label) VALUES (1, ?), (2, ?)");
    $stmt->execute(array('a', 'b'));
    $stmt = $db->prepare("SELECT id, label FROM {$table} WHERE id = :placeholder AND label = (SELECT label AS 'SELECT' FROM {$table} WHERE id = ?)");
    $stmt->execute(array(1, 1));
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_prepare_native_mixed_style');
?>
--EXPECTF--
Warning: PDO::prepare(): SQLSTATE[HY093]: Invalid parameter number: mixed named and positional parameters in %s on line %d

Warning: PDO::prepare(): SQLSTATE[HY093]: Invalid parameter number in %s on line %d

Fatal error: Uncaught Error: Call to a member function execute() on false in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
