--TEST--
Sqlite3Stmt::explain/setExplain usage
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Darwin") die("skip on darwin for now");
$version = SQLite3::version()['versionNumber'];
if ($version <= 3043000) die("skip for sqlite3 < 3.43.0");
?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$db->exec('CREATE TABLE test_explain (a string);');
$stmt = $db->prepare('INSERT INTO test_explain VALUES ("first insert"), ("second_insert")');
$stmt->setExplain(Sqlite3Stmt::EXPLAIN_MODE_EXPLAIN);
var_dump($stmt->explain() == Sqlite3Stmt::EXPLAIN_MODE_EXPLAIN);
$r = $stmt->execute();
$result = [];
while (($arr = $r->fetchArray(SQLITE3_ASSOC)) !== false) $result[] = $arr;
var_dump($result);
$stmts = $db->prepare('SELECT * FROM test_explain');
$stmts->setExplain(Sqlite3Stmt::EXPLAIN_MODE_EXPLAIN_QUERY_PLAN);
$r = $stmts->execute();
$result = [];
while (($arr = $r->fetchArray(SQLITE3_ASSOC)) !== false) $result[] = $arr;
var_dump($result);

$stmt = $db->prepare('INSERT INTO test_explain VALUES ("first insert"), ("second_insert")');
$stmt->setExplain(Sqlite3Stmt::EXPLAIN_MODE_PREPARED);
$stmt->execute();
$stmts = $db->prepare('SELECT * FROM test_explain');
$stmts->setExplain(Sqlite3Stmt::EXPLAIN_MODE_PREPARED);
$r = $stmts->execute();
$result = [];
while (($arr = $r->fetchArray(SQLITE3_ASSOC)) !== false) $result[] = $arr;
var_dump($result);

try {
    $stmts->setExplain(-1);
} catch (\ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    $stmts->setExplain(256);
} catch (\ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

var_dump($stmts->explain() == Sqlite3Stmt::EXPLAIN_MODE_PREPARED);
?>
--EXPECTF--
bool(true)
array(%d) {
  [0]=>
  array(8) {
    ["addr"]=>
    int(0)
    ["opcode"]=>
    string(4) "Init"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(%d)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    %a
  }
  [1]=>
  array(8) {
    ["addr"]=>
    int(1)
    ["opcode"]=>
    string(%d) "%s"
    ["p1"]=>
    int(3)
    ["p2"]=>
    int(%d)
    ["p3"]=>
    int(2)
    ["p4"]=>
    %s
    ["p5"]=>
    int(0)
    ["comment"]=>
    %a
  }
  %A
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  %A
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  %A
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
    ["opcode"]=>
    string(5) "Yield"
    ["p1"]=>
    int(3)
    ["p2"]=>
    int(%d)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
    ["opcode"]=>
    string(4) "Goto"
    ["p1"]=>
    int(0)
    ["p2"]=>
    int(%d)
    ["p3"]=>
    int(0)
    ["p4"]=>
    NULL
    ["p5"]=>
    int(0)
    ["comment"]=>
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
  [%d]=>
  array(8) {
    ["addr"]=>
    int(%d)
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
    %a
  }
}
array(1) {
  [0]=>
  array(4) {
    ["id"]=>
    int(2)
    ["parent"]=>
    int(0)
    ["notused"]=>
    int(%d)
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
SQLite3Stmt::setExplain(): Argument #1 ($mode) must be one of the SQLite3Stmt::EXPLAIN_MODE_* constants
SQLite3Stmt::setExplain(): Argument #1 ($mode) must be one of the SQLite3Stmt::EXPLAIN_MODE_* constants
bool(true)
