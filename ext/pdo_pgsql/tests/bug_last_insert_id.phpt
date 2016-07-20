--TEST--
currval() vs lastval()
PDO PgSQL Bug #1134 [BUG] New record, PostgreSQL and the Primary key https://github.com/phalcon/cphalcon/issues/1134
Fix PHP original PDO_PGSQL::lastInsertId https://github.com/doctrine/dbal/pull/2329
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

$config['ENV']['PDOTEST_DSN'] = 'pgsql:host=localhost port=5432 dbname=test user=postgres password=postgres';
$config['ENV']['PDOTEST_USER'] = 'postgres';
$config['ENV']['PDOTEST_PASS'] = 'postgres';

$db = PDOTest::factory();

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

@$db->query('CREATE TABLE test_last_id (id SERIAL NOT NULL, field1 VARCHAR(10))');

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
?>
--EXPECTREGEX--
string\([0-9]*\)\ \"[0-9]*\"
string\([0-9]*\)\ \"[0-9]*\"
