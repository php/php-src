--TEST--
Variance error in the presence of autoloading (5)
--FILE--
<?php

spl_autoload_register(function ($class) {
    if ($class == 'A') {
        class A
        {
            public function method(): X
            {
            }
        }
        var_dump(new A());
    } elseif ($class == 'B') {
        class B extends A
        {
            public function method(): Y
            {
            }
        }
        var_dump(new B());
    } elseif ($class == 'X') {
        class X
        {
            public function method(Y $a)
            {
            }
        }
        var_dump(new X());
    } elseif ($class == 'Y') {
        class Y extends X
        {
            public function method(Z $a)
            {
            }
        }
        var_dump(new Y());
    } elseif ($class == 'Z') {
        class Z extends Y
        {
            public function method($a)
            {
            }
        }
        var_dump(new Z());
    }
});

var_dump(new B());

?>
--EXPECTF--
object(A)#2 (0) {
}
object(X)#2 (0) {
}

Fatal error: Declaration of Y::method(Z $a) must be compatible with X::method(Y $a) in %s on line %d
