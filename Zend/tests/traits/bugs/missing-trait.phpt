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
Fatal error: Trait "THello" not found in %s on line %d
