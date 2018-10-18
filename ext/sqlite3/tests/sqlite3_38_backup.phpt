--TEST--
SQLite3::backup test
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$db->enableExceptions(true);

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

echo "Closing DB1\n";
var_dump($db->close());

echo "Checking backup contents\n";
var_dump($db2->querySingle('SELECT a FROM test;'));
var_dump($db2->querySingle('SELECT b FROM test;'));

echo "Closing DB2\n";
var_dump($db2->close());

echo "Done\n";
?>
--EXPECTF--
Creating table
Checking if table has been created
int(1)
Backup to DB2
bool(true)
Checking if table has been copied
int(1)
Closing DB1
bool(true)
Checking backup contents
int(42)
string(3) "php"
Closing DB2
bool(true)
Done
