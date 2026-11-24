--TEST--
Operable class cannot inherit non-final concrete operator method from non-Operable ancestor
--FILE--
<?php

readonly class Base1 {
    public function __add(int $x): self {
        return $this;
    }
}

readonly class Base2 extends Base1 {
}

readonly class Child extends Base2 implements Operable {
}

?>
--EXPECTF--
Fatal error: Operable class Child cannot inherit non-final operator method Base1::__add() in %s on line %d
