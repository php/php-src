--TEST--
Bug #81192: "Declaration should be compatible with" gives incorrect line number with traits
--FILE--
<?php

require __DIR__ . '/bug81192_trait.inc';

class A {
    public function foo(): int {
        return 2;
    }
}

class B extends A {
    use T;
}

?>
--EXPECTF--
Fatal error: Declaration of T::foo(): string must be compatible with A::foo(): int in %sbug81192_trait.inc on line 4
