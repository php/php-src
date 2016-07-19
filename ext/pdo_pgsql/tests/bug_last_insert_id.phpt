--TEST--
PDO PgSQL Bug #1134 [BUG] New record, PostgreSQL and the Primary key https://github.com/phalcon/cphalcon/issues/1134
[FIX] - Fix PHP original PDO_PGSQL::lastInsertId https://github.com/doctrine/dbal/pull/2329
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
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->query('CREATE TABLE test_last_id (id SERIAL NOT NULL, field1 VARCHAR(10))');

$stmt = $db->prepare("INSERT INTO test_last_id (field1) VALUES ('test')");

var_dump($db->lastInsertId());
?>
--EXPECT--
int(1)
