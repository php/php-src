--TEST--
PDO Common: Bug #77849 (inconsistent state of cloned statament object)
--EXTENSIONS--
pdo
--FILE--
<?php
$stmt = new PDOStatement();

clone $stmt;
?>
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class PDOStatement in %s:4
Stack trace:
#0 {main}
  thrown in %s on line 4

