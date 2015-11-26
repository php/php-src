--TEST--
Trying instantiate a PDORow object manually
--SKIPIF--
<?php if (!extension_loaded('pdo')) die('skip'); ?>
--FILE--
<?php

new PDORow;

?>
--EXPECTF--
Fatal error: Uncaught PDOException: You may not create a PDORow manually in %spdorow.php:3
Stack trace:
#0 %spdorow.php(3): PDORow->__construct()
#1 {main}
  thrown in %spdorow.php on line 3
