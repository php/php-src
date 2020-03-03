--TEST--
Visibility enforcement on abstract trait methods
--FILE--
<?php

trait T {
    abstract public function method(int $a, string $b);
}

class C {
    use T;

    /* For backwards-compatiblity reasons, visibility is not enforced here. */
    private function method(int $a, string $b) {}
}

?>
===DONE===
--EXPECT--
===DONE===
