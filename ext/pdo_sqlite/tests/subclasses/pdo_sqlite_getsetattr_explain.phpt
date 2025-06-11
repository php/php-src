--TEST--
Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT usage
--EXTENSIONS--
pdo_sqlite
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Darwin") die("skip on darwin for now");
if (!defined('Pdo\Sqlite::EXPLAIN_MODE_EXPLAIN')) die('skip system sqlite does not support EXPLAIN mode');
?>
--FILE--
<?php

$db = new Pdo\Sqlite('sqlite::memory:');

$db->query('CREATE TABLE test_explain (a string);');
$stmt = $db->prepare('INSERT INTO test_explain VALUES ("first insert"), ("second_insert")');
$stmt->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, Pdo\Sqlite::EXPLAIN_MODE_EXPLAIN);
$r = $stmt->execute();
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmts = $db->prepare('SELECT * FROM test_explain');
$stmts->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, Pdo\Sqlite::EXPLAIN_MODE_EXPLAIN_QUERY_PLAN);
$r = $stmts->execute();
var_dump($stmts->fetchAll(PDO::FETCH_ASSOC));

$stmt = $db->prepare('INSERT INTO test_explain VALUES ("first insert"), ("second_insert")');
$stmt->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, Pdo\Sqlite::EXPLAIN_MODE_PREPARED);
$stmt->execute();
$stmts->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, Pdo\Sqlite::EXPLAIN_MODE_PREPARED);
$r = $stmts->execute();
var_dump($stmts->fetchAll(PDO::FETCH_ASSOC));

try {
    $stmts->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, "EXPLAIN");
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    $stmts->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, -1);
} catch (\ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    $stmts->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, 256);
} catch (\ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
array(8) {
  ["addr"]=>
  int(0)
  ["opcode"]=>
  string(4) "Init"
  ["p1"]=>
  int(0)
  ["p2"]=>
  int(14)
  ["p3"]=>
  int(0)
  ["p4"]=>
  NULL
  ["p5"]=>
  int(0)
  ["comment"]=>
  NULL
}
array(1) {
  [0]=>
  array(4) {
    ["id"]=>
    int(2)
    ["parent"]=>
    int(0)
    ["notused"]=>
    int(0)
    ["detail"]=>
    string(17) "SCAN test_explain"
  }
}
array(2) {
  [0]=>
  array(1) {
    ["a"]=>
    string(12) "first insert"
  }
  [1]=>
  array(1) {
    ["a"]=>
    string(13) "second_insert"
  }
}
explain mode must be of type int
explain mode must be one of the EXPLAIN_MODE_* constants
explain mode must be one of the EXPLAIN_MODE_* constants
