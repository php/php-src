--TEST--
Visibility enforcement on abstract trait methods
--FILE--
<?php

trait T {
    abstract public function method(int $a, string $b);
}

class C {
    use T;

    private function method(int $a, string $b) {}
}

?>
--EXPECTF--
Fatal error: Access level to C::method() must be public (as in class T) in %s on line %d
