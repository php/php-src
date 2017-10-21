--TEST--
PDO_DBLIB: RPC Support
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

$st = $db->prepare('select :test as test');
var_dump($st->getAttribute(PDO::DBLIB_ATTR_RPC));
$st->execute([':test' => 1]);
var_dump($st->fetch(PDO::FETCH_ASSOC));

$st = $db->prepare('sp_help', [PDO::DBLIB_ATTR_RPC => 1]);
var_dump($st->getAttribute(PDO::DBLIB_ATTR_RPC));
$prm = 'sp_help';
$st->bindParam('objname', $prm);
$st->execute();
var_dump($st->fetch(PDO::FETCH_ASSOC));
?>
--EXPECTF--
bool(false)
array(1) {
  ["test"]=>
  string(1) "1"
}
bool(true)
array(4) {
  ["Name"]=>
  string(7) "sp_help"
  ["Owner"]=>
  string(3) "sys"
  ["Type"]=>
  string(16) "stored procedure"
  ["Created_datetime"]=>
  string(%d) "%s"
}
