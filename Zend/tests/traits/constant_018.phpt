--TEST--
Referencing trait constants directly on calling \constant() causes a fatal error
--FILE--
<?php

trait TestTrait {
  public const Constant = 42;
}

var_dump(\constant('TestTrait::Constant'));
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access trait constant TestTrait::Constant directly in %s:%d
Stack trace:
#0 %s: constant('TestTrait::Cons...')
#1 {main}
  thrown in %s on line %d
