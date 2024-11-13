--TEST--
MySQL PDO->prepare(), native PS, named placeholder
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

    $table = 'pdo_mysql_prepare_native_column';
    MySQLPDOTest::createTestTable($table, $db);

    $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
    if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
        printf("[002] Unable to turn off emulated prepared statements\n");

    $stmt = $db->prepare("SELECT :param FROM {$table} ORDER BY id ASC LIMIT 1");
    $stmt->execute(array(':param' => 'id'));
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    $db->prepare("SELECT :placeholder FROM {$table} WHERE :placeholder > :placeholder");
    $stmt->execute(array(':placeholder' => 'test'));

    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_prepare_native_column');
?>
--EXPECTF--
array(1) {
  [0]=>
  array(1) {
    ["?"]=>
    string(2) "id"
  }
}

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: parameter was not defined in %s on line %d
array(0) {
}
done!
