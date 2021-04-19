--TEST--
Partial application return type
--FILE--
<?php
function foo() : array {}

echo (string) new ReflectionFunction(foo(...));
?>
--EXPECTF--
Function [ <user> partial function foo ] {
  @@ %s 4 - 4

  - Parameters [0] {
  }
  - Return [ array ]
}

