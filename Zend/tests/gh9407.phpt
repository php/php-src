--TEST--
GH-9407: LSP error in eval'd code refers to wrong class for static type
--FILE--
<?php

class A {
    public function duplicate(): static {}
}

class C {
    public static function generate() {
        eval(<<<PHP
            class B extends A {
                public function duplicate(): A {}
            }
        PHP);
    }
}

C::generate();

?>
--EXPECTF--
Fatal error: Declaration of B::duplicate(): A must be compatible with A::duplicate(): static in %s : eval()'d code on line %d
