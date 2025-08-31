--TEST--
PDO_firebird subclass basic
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

require_once __DIR__ . "/testdb.inc";

$db = getDbConnection();

$db->query('CREATE TABLE pdofirebird_001 (idx INT NOT NULL PRIMARY KEY, name VARCHAR(20))');
$db->query("INSERT INTO pdofirebird_001 VALUES (1, 'PHP')");
$db->query("INSERT INTO pdofirebird_001 VALUES (2, 'PHP6')");

foreach ($db->query('SELECT name FROM pdofirebird_001') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require_once __DIR__ . '/testdb.inc';
$dbh = getDbConnection();
@$dbh->exec("DROP TABLE pdofirebird_001");
unset($dbh);
?>
--EXPECT--
array(2) {
  ["NAME"]=>
  string(3) "PHP"
  [0]=>
  string(3) "PHP"
}
array(2) {
  ["NAME"]=>
  string(4) "PHP6"
  [0]=>
  string(4) "PHP6"
}
Fin.
