--TEST--
Bug #64705 errorInfo property of PDOException is null when PDO::__construct() fails
--SKIPIF--
<?php
if (!extension_loaded('pdo_pgsql')) print 'skip not loaded';
?>
--FILE--
<?php
$dsn = 'pgsql:host=DonotExistsHost;dbname=test;user=foo;password=wrongpass';
try {
  $pdo = new \PDO($dsn, null, null);
} catch (\PDOException $e) {
  var_dump(!empty($e->errorInfo) && is_array($e->errorInfo));
}
?>
--EXPECTF--
bool(true)