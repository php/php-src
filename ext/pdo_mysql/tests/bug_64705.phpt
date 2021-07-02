--TEST--
Bug #64705 errorInfo property of PDOException is null when PDO::__construct() fails
--EXTENSIONS--
pdo_mysql
--FILE--
<?php
$dsn = 'mysql:host=DonotExistsHost;dbname=test;user=foo;password=wrongpass';
try {
  $pdo = new \PDO($dsn, null, null);
} catch (\PDOException $e) {
  var_dump(!empty($e->errorInfo) && is_array($e->errorInfo));
}
?>
--EXPECT--
bool(true)
