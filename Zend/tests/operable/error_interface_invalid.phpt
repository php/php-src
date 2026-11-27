--TEST--
Invalid operator method declared on interface extending Operable is rejected
--FILE--
<?php

interface BadInterface extends Operable {
    public function __mul(array $a): self;
}

?>
--EXPECTF--
Fatal error: Operator method BadInterface::__mul() has an unsupported operand parameter type in %s on line %d
