--TEST--
Bug #64705 errorInfo property of PDOException is null when PDO::__construct() fails
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$dsn = 'sqlite:./bug64705NonExistingDir/bug64705NonExistingDb';
try {
  $pdo = new \PDO($dsn, null, null);
} catch (\PDOException $e) {
  var_dump(!empty($e->errorInfo) && is_array($e->errorInfo));
}
?>
--EXPECT--
bool(true)
