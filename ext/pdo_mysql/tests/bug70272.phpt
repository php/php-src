--TEST--
Bug #70272 (Segfault in pdo_mysql)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
$a = new stdClass();
$a->a = &$a;
require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$dummy = new stdClass();

$db = MySQLPDOTest::factory();
$dummy = null;

$a->c = $db;
$a->b = $db->prepare("select 1");
$a->d = $db->prepare("select 2");
$a->e = $db->prepare("select 3");
$a->f = $db->prepare("select 4");
gc_disable();
?>
okey
--EXPECT--
okey
