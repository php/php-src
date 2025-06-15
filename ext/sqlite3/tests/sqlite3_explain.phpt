--TEST--
Sqlite3Stmt::explain/setExplain usage
--EXTENSIONS--
sqlite
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
while (($arr = $r->fetchArray(SQLITE3_ASSOC)) !== false) var_dump($arr);
var_dump($r->fetchArray(SQLITE3_ASSOC));
$stmts = $db->prepare('SELECT * FROM test_explain');
$stmts->setExplain(Sqlite3Stmt::EXPLAIN_MODE_EXPLAIN_QUERY_PLAN);
var_dump($stmt->explain() == Sqlite3Stmt::EXPLAIN_MODE_EXPLAIN_QUERY_PLAN);
$r = $stmts->execute();
while (($arr = $r->fetchArray(SQLITE3_ASSOC)) !== false) var_dump($arr);

$stmt = $db->prepare('INSERT INTO test_explain VALUES ("first insert"), ("second_insert")');
$stmt->setExplain(Sqlite3Stmt::EXPLAIN_MODE_PREPARED);
$stmt->execute();
$stmts->setExplain(Sqlite3Stmt::EXPLAIN_MODE_PREPARED);
$r = $stmts->execute();
while (($arr = $r->fetchArray(SQLITE3_ASSOC)) !== false) var_dump($arr);

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
--EXPECT--
bool(true)
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
bool(false)
bool(false)
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
SQLite3Stmt::setExplain(): Argument #1 ($mode) must be one of the SQLite3Stmt::EXPLAIN_MODE_* constants
SQLite3Stmt::setExplain(): Argument #1 ($mode) must be one of the SQLite3Stmt::EXPLAIN_MODE_* constants
bool(false)
