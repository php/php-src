--TEST--
Bug #70272 (Segfault in pdo_mysql)
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--INI--
report_memleaks=off
--FILE--
<?php
$a = new Stdclass();
$a->a = &$a;
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$dummy = new StdClass();

$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$dummy = NULL;

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
