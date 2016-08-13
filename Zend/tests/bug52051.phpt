--TEST--
Bug #52051 (handling of case sensitivity of old-style constructors changed in 5.3+)
--FILE--
<?php

class AA {
    function AA() { echo "foo\n"; }
}
class bb extends AA {}
class CC extends bb {
   function CC() { parent::bb(); }
}
new CC();

class A {
    function A() { echo "bar\n"; }
}
class B extends A {}
class C extends B {
   function C() { parent::B(); }
}
new C();

?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; AA has a deprecated constructor in %s on line %d

Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; CC has a deprecated constructor in %s on line %d

Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; A has a deprecated constructor in %s on line %d

Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; C has a deprecated constructor in %s on line %d
foo
bar
