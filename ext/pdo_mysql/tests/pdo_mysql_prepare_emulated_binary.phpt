--TEST--
MySQL PDO->prepare(), no warnings should be raised for binary values using emulated PS
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
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

    // Force the connection to utf8, which is enough to make the test fail
    // MySQL 5.6+ would be required for utf8mb4
    $db->exec("SET NAMES 'utf8'");

    $content = '0191D886E6DC73E7AF1FEE7F99EC6235';

    $statement = $db->prepare('SELECT HEX(?) as test');
    $statement->bindValue(1, hex2bin($content), PDO::PARAM_LOB);
    $statement->execute();

    var_dump($statement->fetchAll(PDO::FETCH_ASSOC)[0]['test'] === $content);
    var_dump($db->query('SHOW WARNINGS')->fetchAll(PDO::FETCH_ASSOC));

    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

    $statement2 = $db->prepare('SELECT HEX(?) as test');
    $statement2->bindValue(1, hex2bin($content), PDO::PARAM_LOB);
    $statement2->execute();

    var_dump($statement2->fetchAll(PDO::FETCH_ASSOC)[0]['test'] === $content);

    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true); // SHOW WARNINGS can only be used when PDO::ATTR_EMULATE_PREPARES=true
    var_dump($db->query('SHOW WARNINGS')->fetchAll(PDO::FETCH_ASSOC));
    print "done!";
?>
--EXPECTF--
bool(true)
array(0) {
}
bool(true)
array(0) {
}
done!
