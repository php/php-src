--TEST--
PDO_DBLIB: Does not support get column meta
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

$stmt = $db->prepare("select ic1.* from information_schema.columns ic1");
$stmt->execute();
echo "native_type:\n";
var_dump($stmt->getColumnMeta(0)["native_type"]);
echo "name:\n";
var_dump($stmt->getColumnMeta(0)["name"]);
echo "len:\n";
var_dump($stmt->getColumnMeta(0)["len"]);
echo "precision:\n";
var_dump($stmt->getColumnMeta(0)["precision"]);
echo "pdo_type:\n";
var_dump($stmt->getColumnMeta(0)["pdo_type"]);
$stmt = null;
?>
--EXPECTF--
native_type:
string(%d) "%s"
name:
string(%d) "%s"
len:
int(%d)
precision:
int(%d)
pdo_type:
int(%d)
