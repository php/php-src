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
Fatal error: Method C::method() must be static to be compatible with overridden method T::method() in %s on line %d
