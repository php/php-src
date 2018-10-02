--TEST--
Bug #76896 (Last insert ID overwritten after SELECT statement)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip');
require dirname(__FILE__) . '/config.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$db->query("DROP TABLE IF EXISTS `test`;");
$db->query("CREATE TABLE `test` (
				`id` int(11) NOT NULL AUTO_INCREMENT,
				`label` char(1) NOT NULL,
				PRIMARY KEY (`id`)
			) ENGINE=InnoDB  DEFAULT CHARSET=utf8;");

$db->query("INSERT INTO test(label) VALUES ('x')");
var_dump($db->lastInsertId());

$db->query("SELECT * FROM test LIMIT 1");
var_dump($db->lastInsertId());

$db->query("DROP TABLE IF EXISTS `test`;");
?>
--EXPECT--
string(1) "1"
string(1) "1"
