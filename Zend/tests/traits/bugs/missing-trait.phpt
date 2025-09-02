--TEST--
Check error message for missing traits
--FILE--
<?php
error_reporting(E_ALL);

class TraitsTest {
  use THello;
}

$test = new TraitsTest();

?>
--EXPECTF--
Fatal error: Uncaught Error: Trait "THello" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
