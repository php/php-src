--TEST--
GH-7792 (Refer to enum as enum instead of class)
--FILE--
<?php

interface A {
    public function a(): void;
}

enum B implements A {}

?>
--EXPECTF--
Fatal error: Enum B must implement 1 abstract private method (A::a) in %s on line %d
