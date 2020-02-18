--TEST--
SQLite3Result::fetchArray() test, testing non-SQLITE3_ROWS result type
--CREDITS--
Antoni Villalonga Noceras <antoni@friki.cat>
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE foo (bar STRING)');

$result = $db->query("INSERT INTO foo (bar) VALUES ('This is a test')");

//fetchArray should not call sqlite3_step() after an Insert, Update, etc
$result->fetchArray(SQLITE3_ASSOC);

$result = $db->query("SELECT COUNT(*) FROM foo");
echo $result->fetchArray(SQLITE3_NUM)[0];
?>
--EXPECTF--
1
