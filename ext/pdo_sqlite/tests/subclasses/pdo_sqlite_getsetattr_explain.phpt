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
var_dump($stmt->getAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT) == Pdo\Sqlite::EXPLAIN_MODE_EXPLAIN);
$r = $stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
$stmts = $db->prepare('SELECT * FROM test_explain');
$stmts->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, Pdo\Sqlite::EXPLAIN_MODE_EXPLAIN_QUERY_PLAN);
var_dump($stmt->getAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT) == Pdo\Sqlite::EXPLAIN_MODE_EXPLAIN_QUERY_PLAN);
$r = $stmts->execute();
var_dump($stmts->fetchAll(PDO::FETCH_ASSOC));

$stmt = $db->prepare('INSERT INTO test_explain VALUES ("first insert"), ("second_insert")');
$stmt->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, Pdo\Sqlite::EXPLAIN_MODE_PREPARED);
$stmt->execute();
$stmts->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, Pdo\Sqlite::EXPLAIN_MODE_PREPARED);
$r = $stmts->execute();
var_dump($stmts->fetchAll(PDO::FETCH_ASSOC));

class Duh {}

try {
    $stmts->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, "EXPLAIN");
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    $stmts->setAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT, new Duh());
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

var_dump($stmts->getAttribute(Pdo\Sqlite::ATTR_EXPLAIN_STATEMENT) == Pdo\Sqlite::EXPLAIN_MODE_PREPARED);
?>
--EXPECT--
bool(true)
array(16) {
  [0]=>
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
  [1]=>
  array(8) {
    ["addr"]=>
    int(1)
    ["opcode"]=>
    string(13) "InitCoroutine"
    ["p1"]=>
    int(3)
    ["p2"]=>
    int(7)
    ["p3"]=>
    int(2)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [2]=>
  array(8) {
    ["addr"]=>
    int(2)
    ["opcode"]=>
    string(7) "String8"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(2)
    ["p3"]=>
    int(0)
    ["p4"]=>
    string(12) "first insert"
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [3]=>
  array(8) {
    ["addr"]=>
    int(3)
    ["opcode"]=>
    string(5) "Yield"
    ["p1"]=>
    int(3)
    ["p2"]=>
    int(0)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [4]=>
  array(8) {
    ["addr"]=>
    int(4)
    ["opcode"]=>
    string(7) "String8"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(2)
    ["p3"]=>
    int(0)
    ["p4"]=>
    string(13) "second_insert"
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [5]=>
  array(8) {
    ["addr"]=>
    int(5)
    ["opcode"]=>
    string(5) "Yield"
    ["p1"]=>
    int(3)
    ["p2"]=>
    int(0)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [6]=>
  array(8) {
    ["addr"]=>
    int(6)
    ["opcode"]=>
    string(12) "EndCoroutine"
    ["p1"]=>
    int(3)
    ["p2"]=>
    int(0)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [7]=>
  array(8) {
    ["addr"]=>
    int(7)
    ["opcode"]=>
    string(9) "OpenWrite"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(2)
    ["p3"]=>
    int(0)
    ["p4"]=>
    string(1) "1"
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [8]=>
  array(8) {
    ["addr"]=>
    int(8)
    ["opcode"]=>
    string(5) "Yield"
    ["p1"]=>
    int(3)
    ["p2"]=>
    int(13)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [9]=>
  array(8) {
    ["addr"]=>
    int(9)
    ["opcode"]=>
    string(8) "NewRowid"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(1)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [10]=>
  array(8) {
    ["addr"]=>
    int(10)
    ["opcode"]=>
    string(10) "MakeRecord"
    ["p1"]=>
    int(2)
    ["p2"]=>
    int(1)
    ["p3"]=>
    int(4)
    ["p4"]=>
    string(1) "C"
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [11]=>
  array(8) {
    ["addr"]=>
    int(11)
    ["opcode"]=>
    string(6) "Insert"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(4)
    ["p3"]=>
    int(1)
    ["p4"]=>
    string(12) "test_explain"
    ["p5"]=>
    int(57)
    ["comment"]=>
    NULL
  }
  [12]=>
  array(8) {
    ["addr"]=>
    int(12)
    ["opcode"]=>
    string(4) "Goto"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(8)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [13]=>
  array(8) {
    ["addr"]=>
    int(13)
    ["opcode"]=>
    string(4) "Halt"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(0)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
  [14]=>
  array(8) {
    ["addr"]=>
    int(14)
    ["opcode"]=>
    string(11) "Transaction"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(1)
    ["p3"]=>
    int(1)
    ["p4"]=>
    string(1) "0"
    ["p5"]=>
    int(1)
    ["comment"]=>
    NULL
  }
  [15]=>
  array(8) {
    ["addr"]=>
    int(15)
    ["opcode"]=>
    string(4) "Goto"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(1)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    NULL
  }
}
bool(false)
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
explain mode must be of type int, string given
explain mode must be of type int, Duh given
explain mode must be one of the Pdo\Sqlite::EXPLAIN_MODE_* constants
explain mode must be one of the Pdo\Sqlite::EXPLAIN_MODE_* constants
bool(true)
