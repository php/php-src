--TEST--
Interface delegation: static methods are not supported
--FILE--
<?php

interface A {
    public static function foo(): string;
}

class AImpl implements A {
    public static function foo(): string {
        return 'A';
    }
}

class C implements A by $a {
    public function __construct(protected A $a) {}
}
C::foo();
?>
--EXPECTF--
Fatal error: Uncaught Error: Interface delegation property not found in %s:%d
Stack trace:
#0 %s(%d): A::foo()
#1 {main}
  thrown in %s on line %d
