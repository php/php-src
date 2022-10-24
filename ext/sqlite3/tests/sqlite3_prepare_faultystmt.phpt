--TEST--
SQLite3::prepare test, testing for faulty statement
--CREDITS--
Michelangelo van Dam
# Belgian PHP Testfest 2009
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE foo (id INTEGER, bar STRING)');
$db->exec("INSERT INTO foo (id, bar) VALUES (1, 'This is a test')");

try {
	$stmt = $db->prepare('SELECT foo FROM bar');
} catch (SQLite3Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Unable to prepare statement: no such table: bar
