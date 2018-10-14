--TEST--
PDO PgSQL Bug #69344 (PDO PgSQL Incorrect binding numeric array with gaps)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$pdo->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
$pdo->setAttribute (\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_ASSOC);

$test = function () use ($pdo) {
	$arr = [
		0 => "a",
		2 => "b",
	];

	$stmt = $pdo->prepare("SELECT (?)::text AS a, (?)::text AS b");

	try {
		$stmt->execute($arr);
		var_dump($stmt->fetch());
	} catch (\Exception $e) {
		echo $e->getMessage()."\n";
	}
};

$test();

$pdo->setAttribute(\PDO::ATTR_EMULATE_PREPARES, true);

$test();

?>
--EXPECT--
SQLSTATE[HY093]: Invalid parameter number: parameter was not defined
SQLSTATE[HY093]: Invalid parameter number: parameter was not defined
