--TEST--
Variance error in the presence of autoloading (2)
--FILE--
<?php

// Same as autoload_error1, but for argument types.
spl_autoload_register(function($class) {
    if ($class === 'A') {
        class A {
            public function method(B $x) {}
        }
    } else if ($class == 'B') {
        class B extends A {
            public function method(C $x) {}
        }
    } else {
        class C extends B {
        }
    }
});

$b = new B;
$c = new C;

?>
--EXPECTF--
Warning: Declaration of B::method(C $x) should be compatible with A::method(B $x) in %s on line %d
