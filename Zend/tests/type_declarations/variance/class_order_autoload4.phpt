--TEST--
Class order allowed with autoloading (4)
--FILE--
<?php

// Same as autoload3 test case, but with X, Y being interfaces.
spl_autoload_register(function($class) {
    if ($class == 'A') {
        class A {
            public function method(): X {}
        }
    } else if ($class == 'B') {
        class B extends A {
            public function method(): Y {}
        }
    } else if ($class == 'X') {
        interface X {
            public function method(): A;
        }
    } else if ($class == 'Y') {
        interface Y extends X {
            public function method(): B;
        }
    }
});

$b = new B;

?>
===DONE===
--EXPECT--
===DONE===
