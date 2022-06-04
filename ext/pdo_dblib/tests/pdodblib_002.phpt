--TEST--
PdoDblib create through PDO::connect
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
getDbConnection();
?>
--FILE--
<?php

require __DIR__ . '/config.inc';

$db = connectToDb();
if (!$db instanceof PdoDblib) {
    echo "Wrong class type. Should be PdoDblib but is " . get_class($db) . "\n";
}

$db->query("create table #pdo_dblib_002(name varchar(32))");
$db->query("insert into #pdo_dblib_002 values('PHP'), ('PHP6')");

foreach ($db->query('SELECT name FROM #pdo_dblib_002') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require __DIR__ . '/config.inc';
$db = getDbConnection();
$db->query('DROP TABLE IF EXISTS #pdo_dblib_002');
?>
--EXPECT--
array(2) {
  ["name"]=>
  string(3) "PHP"
  [0]=>
  string(3) "PHP"
}
array(2) {
  ["name"]=>
  string(4) "PHP6"
  [0]=>
  string(4) "PHP6"
}
Fin.
