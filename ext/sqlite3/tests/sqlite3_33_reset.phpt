--TEST--
SQLite3:: reset
--CREDITS--
Ward Hus & James Cauwelier
#@ PHP TESTFEST 2009 (BELGIUM)
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');

$db->exec('CREATE TABLE foo (id INTEGER, bar STRING)');
$db->exec("INSERT INTO foo (id, bar) VALUES (1, 'This is a test')");

$stmt = $db->prepare('SELECT bar FROM foo WHERE id=:id');
$stmt->bindValue(':id', 1, SQLITE3_INTEGER);
$stmt->reset("dummy");
$stmt->reset();

//var_dump($db);
//var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
Warning: SQLite3Stmt::reset() expects exactly 0 parameters, 1 given in %s on line %d
Done
