--TEST--
PdoDblib basic
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
$db = getDbConnection(PdoDblib::class);

$db->query("CREATE TABLE #pdo_dblib_001(name VARCHAR(32)); ");
$db->query("INSERT INTO #pdo_dblib_001 VALUES('PHP'), ('PHP6');");

foreach ($db->query('SELECT name FROM #pdo_dblib_001') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require __DIR__ . '/config.inc';
$db = getDbConnection();
$db->query('DROP TABLE IF EXISTS #pdo_dblib_001');
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
