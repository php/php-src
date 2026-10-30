--TEST--
PDO PgSQL PDOStatement::getAttribute(Pgsql::ATTR_RESULT_MEMORY_SIZE)
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
if (!defined('Pgsql::ATTR_RESULT_MEMORY_SIZE')) die('skip constant Pgsql::ATTR_RESULT_MEMORY_SIZE does not exist');
--FILE--
<?php

require_once __DIR__ . "/config.inc";

/** @var Pdo */
$db =  Pdo::connect($config['ENV']['PDOTEST_DSN']);

echo 'Result set with only 1 row: ';
$statement = $db->query('select 1');
$result_1 = $statement->getAttribute(Pgsql::ATTR_RESULT_MEMORY_SIZE);
var_dump($result_1);

echo 'Result set with many rows: ';
$result = $db->query('select generate_series(1, 10000)');
$result_2 = $result->getAttribute(Pgsql::ATTR_RESULT_MEMORY_SIZE);
var_dump($result_2);

echo 'Large result sets should require more memory than small ones: ';
var_dump($result_2 > $result_1);

echo 'Statements that are not executed should not consume memory: ';
$statement = $db->prepare('select 1');
$result_3 = $statement->getAttribute(Pgsql::ATTR_RESULT_MEMORY_SIZE);
var_dump($result_3);

echo 'and should emit Error: ';
printf("%s: %d: %s\n", ...$statement->errorInfo());

--EXPECTF--
Result set with only 1 row: int(%d)
Result set with many rows: int(%d)
Large result sets should require more memory than small ones: bool(true)
Statements that are not executed should not consume memory: NULL
and should emit Error: HY000: 0: statement '%s' has not been executed yet
