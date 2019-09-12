--TEST--
Bug #30519 (Interface not existing says Class not found)
--FILE--
<?php
class test implements a {
}
?>
--EXPECTF--
Fatal error: Uncaught Error: Interface 'a' not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
