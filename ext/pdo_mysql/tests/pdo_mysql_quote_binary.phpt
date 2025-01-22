--TEST--
MySQL PDO->quote(), properly handle binary data
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

    $content = "\xC3\xA1\xC3";
    $quoted = $db->quote($content, PDO::PARAM_LOB);

    var_dump($quoted);
    var_dump($db->query("SELECT HEX({$quoted})")->fetch(PDO::FETCH_NUM)[0]);
?>
--EXPECTF--
string(%d) "_binary'%s'"
string(6) "C3A1C3"
