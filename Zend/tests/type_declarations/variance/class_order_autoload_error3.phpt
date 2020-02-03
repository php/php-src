--TEST--
Variance error in the presence of autoloading (3)
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
    } elseif ($class == 'B') {
        class B extends A
        {
            public function method(): Y
            {
            }
        }
    } elseif ($class == 'X') {
        class X
        {
            public function method(): Q
            {
            }
        }
    } elseif ($class == 'Y') {
        class Y extends X
        {
            public function method(): R
            {
            }
        }
    } elseif ($class == 'Q') {
        class Q
        {
            public function method(): B
            {
            }
        }
    } elseif ($class == 'R') {
        class R extends Q
        {
            public function method(): A
            {
            }
        }
    }
});

$b = new B();

?>
--EXPECTF--
Fatal error: Declaration of R::method(): A must be compatible with Q::method(): B in %s on line %d
