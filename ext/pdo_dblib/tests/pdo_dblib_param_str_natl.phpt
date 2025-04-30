--TEST--
PDO_DBLIB: national character set values are quoted correctly in queries
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

$db = getDbConnection();

$stmt = $db->prepare('SELECT :value');
$stmt->bindValue(':value', 'foo', PDO::PARAM_STR | PDO::PARAM_STR_NATL);
$stmt->execute();

var_dump($stmt->debugDumpParams());
?>
--EXPECT--
SQL: [13] SELECT :value
Sent SQL: [13] SELECT N'foo'
Params:  1
Key: Name: [6] :value
paramno=-1
name=[6] ":value"
is_param=1
param_type=1073741826
NULL
