--TEST--
Grandchild cannot override final operator method inherited through Operable child from non-Operable base
--FILE--
<?php

readonly class Base {
    final public function __add(int $x): self {
        return $this;
    }
}

readonly class Child extends Base implements Operable {
}

readonly class Grand extends Child {
    public function __add(int $x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Cannot override final method Base::__add() in %s on line %d
