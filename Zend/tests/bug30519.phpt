--TEST--
Bug #30519 (Interface not existing says Class not found)
--FILE--
<?php
class test implements a {
}
?>
--EXPECTF--
Fatal error: Uncaught Error: Interface 'a' not found in %sbug30519.php:2
Stack trace:
#0 {main}
  thrown in %sbug30519.php on line 2

