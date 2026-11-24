--TEST--
Early-bound subclass of Operable parent is validated for operator methods
--FILE--
<?php

abstract readonly class BaseOp implements Operable {
    abstract public function __add(int $x): self;
}

final readonly class BadChildOp extends BaseOp {
    public function __add(int $x): self {
        return $this;
    }

    public function __mul(string $bad): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method BadChildOp::__mul() has an unsupported operand parameter type in %s on line %d
