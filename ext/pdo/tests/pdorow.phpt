--TEST--
Trying instantiate a PDORow object manually
--EXTENSIONS--
pdo
--FILE--
<?php

new PDORow;

?>
--EXPECTF--
Fatal error: Uncaught Error: A PDORow class cannot be manually instantiated in %s:%d
Stack trace:
#0 {main}
  thrown in %spdorow.php on line 3
