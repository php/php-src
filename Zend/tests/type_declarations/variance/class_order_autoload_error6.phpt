--TEST--
Variance error in the presence of autoloading (6)
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
            public function method(): X {}
        }
        var_dump(new X);
    } else if ($class == 'Y') {
        class Y extends X {
            public function method(): Unknown {}
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

Fatal error: Could not check compatibility between Y::method(): Unknown and X::method(): X, because class Unknown is not available in %s on line %d
