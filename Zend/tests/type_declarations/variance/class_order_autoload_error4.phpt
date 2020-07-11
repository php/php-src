--TEST--
Variance error in the presence of autoloading (4)
--FILE--
<?php

spl_autoload_register(function($class) {
    if ($class == 'A') {
        class A {
            public function method(): X {}
        }
        var_dump(new A);
    } else if ($class == 'B') {
        class B extends A {
            public function method(): Y {}
        }
        var_dump(new B);
    } else if ($class == 'X') {
        class X {
            public function method(): B {}
        }
        var_dump(new X);
    } else if ($class == 'Y') {
        class Y extends X {
            public function method(): A {}
        }
        var_dump(new Y);
    }
});

var_dump(new B);

?>
--EXPECTF--
object(A)#2 (0) {
}
object(X)#2 (0) {
}

Fatal error: Declaration of Y::method(): A must be compatible with X::method(): B in %s on line %d
