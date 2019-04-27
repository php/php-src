<?php
if (!$db) {
	require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
	$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
}
// $db->exec('DROP TABLE IF EXISTS test');
$db->exec('CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE=' . $engine);
$db->exec('INSERT INTO test(id, label) VALUES (1, "a"), (2, "b"), (3, "c"), (4, "d"), (5, "e"), (6, "f")');
?>
