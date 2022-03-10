--TEST--
GH-7792 (Refer to enum as enum instead of class)
--FILE--
<?php

interface A {
    const FOO = 'foo';
}

interface B {
    const FOO = 'foo';
}

enum Foo implements A, B {}

?>
--EXPECTF--
Fatal error: Enum Foo inherits both A::FOO and B::FOO, which is ambiguous in %s on line %d
