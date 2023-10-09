--TEST--
PDO PgSQL Bug #72633 (Postgres PDO lastInsertId() should work without specifying a sequence)
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';

$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->query('CREATE TABLE test_last_id_72633 (id SERIAL NOT NULL, field1 VARCHAR(10))');

$stmt = $db->prepare("INSERT INTO test_last_id_72633 (field1) VALUES ('test')");

$stmt->execute();

/**
 * No sequence name informed
 */
var_dump($db->lastInsertId());
/**
 * Sequence name informed
 */
var_dump($db->lastInsertId('test_last_id_72633_id_seq'));

?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->query('DROP TABLE IF EXISTS test_last_id_72633');
?>
--EXPECTREGEX--
string\([0-9]*\)\ \"[0-9]*\"
string\([0-9]*\)\ \"[0-9]*\"
