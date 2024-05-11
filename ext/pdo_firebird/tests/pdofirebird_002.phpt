--TEST--
PDO_firebird connect through PDO::connect
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require(__DIR__ . '/skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

require_once __DIR__ . "/testdb.inc";

$db = connectToDb();
if (!$db instanceof PdoFirebird) {
    echo "Wrong class type. Should be PdoFirebird but is " . get_class($db) . "\n";
}

$db->query('CREATE TABLE pdofirebird_002 (idx INT NOT NULL PRIMARY KEY, name VARCHAR(20))');

$db->exec("INSERT INTO pdofirebird_002 VALUES(1, 'A')");
$db->exec("INSERT INTO pdofirebird_002 VALUES(2, 'B')");
$db->exec("INSERT INTO pdofirebird_002 VALUES(3, 'C')");

foreach ($db->query('SELECT name FROM pdofirebird_002') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require_once __DIR__ . "/testdb.inc";
$dbh = getDbConnection();
@$dbh->exec("DROP TABLE pdofirebird_002");
unset($dbh);
?>
--EXPECT--
array(2) {
  ["NAME"]=>
  string(1) "A"
  [0]=>
  string(1) "A"
}
array(2) {
  ["NAME"]=>
  string(1) "B"
  [0]=>
  string(1) "B"
}
array(2) {
  ["NAME"]=>
  string(1) "C"
  [0]=>
  string(1) "C"
}
Fin.
