--TEST--
SQLite3::backup test
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

echo "Creating table\n";
$db->exec('CREATE TABLE test (a, b);');
$db->exec('INSERT INTO test VALUES (42, \'php\');');

echo "Checking if table has been created\n";
var_dump($db->querySingle('SELECT COUNT(*) FROM sqlite_master;'));

$db2 = new SQLite3(':memory:');

echo "Backup to DB2\n";
var_dump($db->backup($db2));

echo "Checking if table has been copied\n";
var_dump($db2->querySingle('SELECT COUNT(*) FROM sqlite_master;'));

echo "Checking backup contents\n";
var_dump($db2->querySingle('SELECT a FROM test;'));
var_dump($db2->querySingle('SELECT b FROM test;'));

echo "Resetting DB2\n";

$db2->close();
$db2 = new SQLite3(':memory:');

echo "Locking DB1\n";
var_dump($db->exec('BEGIN EXCLUSIVE;'));

echo "Backup to DB2 (should fail)\n";
var_dump($db->backup($db2));

?>
--EXPECTF--
Creating table
Checking if table has been created
int(1)
Backup to DB2
bool(true)
Checking if table has been copied
int(1)
Checking backup contents
int(42)
string(3) "php"
Resetting DB2
Locking DB1
bool(true)
Backup to DB2 (should fail)

Warning: SQLite3::backup(): Backup failed: source database is busy in %s on line %d
bool(false)