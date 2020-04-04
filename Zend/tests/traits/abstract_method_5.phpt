--TEST--
Staticness enforcement on abstract trait methods
--FILE--
<?php

trait T {
    abstract static public function method(int $a, string $b);
}

class C {
    use T;

    public function method(int $a, string $b) {}
}

?>
--EXPECTF--
Fatal error: Cannot make static method T::method() non static in class C in %s on line %d
