--TEST--
PDO PgSQL Bug #72633 (Postgres PDO lastInsertId() should work without specifying a sequence)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';

$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->query('CREATE TABLE test_last_id (id SERIAL NOT NULL, field1 VARCHAR(10))');

$stmt = $db->prepare("INSERT INTO test_last_id (field1) VALUES ('test')");

$stmt->execute();

/**
 * No sequence name informed
 */
var_dump($db->lastInsertId());
/**
 * Sequence name informed
 */
var_dump($db->lastInsertId('test_last_id_id_seq'));

$db->query('DROP TABLE test_last_id');

?>
--EXPECTREGEX--
string\([0-9]*\)\ \"[0-9]*\"
string\([0-9]*\)\ \"[0-9]*\"
