--TEST--
PDO OCI Bug #55138 (cannot insert more than 1332 one byte chars in al32utf8 varchar2 field)
--SKIPIF--
<?php
/* $Id$ */
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require dirname(__FILE__).'/../../pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');

$db->exec("CREATE TABLE test(test VARCHAR2(2000))");
$statement = $db->prepare("INSERT INTO test VALUES(:test)");
$test = str_repeat("F", 2000);
$statement->bindParam(":test", $test);
$result = $statement->execute();
var_dump($result);

$data = $db->query('SELECT * FROM test')->fetchAll();
$result = ($test === $data[0][0]);
var_dump($result);
--EXPECTF--
bool(true)
bool(true)
