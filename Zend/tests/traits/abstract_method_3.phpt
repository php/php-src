--TEST--
Private abstract method from trait enforced in class
--FILE--
<?php

trait T {
    abstract private function neededByTheTrait(int $a, string $b);
}

class C {
    use T;

    private function neededByTheTrait(array $a, object $b) {}
}

?>
--EXPECTF--
Fatal error: Declaration of C::neededByTheTrait(array $a, object $b) (from T) must be compatible with T::neededByTheTrait(int $a, string $b) in %s on line %d
