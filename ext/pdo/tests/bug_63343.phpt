--TEST--
PDO Common: Bug #63343 (Commit failure for repeated persistent connection)
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

putenv("PDOTEST_ATTR=" . serialize(array(PDO::ATTR_PERSISTENT => true)));

$db = PDOTest::factory('PDO', false);
$db->beginTransaction();
$st = $db->query('select 1');
echo $st->fetchColumn(), PHP_EOL;
$db->commit();

$db = PDOTest::factory('PDO', false);
$db->beginTransaction();
$st = $db->query('select 2');
echo $st->fetchColumn(), PHP_EOL;
$db->commit();
?>
===DONE===
--EXPECT--
1
2
===DONE===
