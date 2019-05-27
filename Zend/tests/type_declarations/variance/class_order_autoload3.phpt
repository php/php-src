--TEST--
Class order allowed with autoloading (3)
--FILE--
<?php

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
        class X {
            public function method(): A {}
        }
    } else if ($class == 'Y') {
        class Y extends X {
            public function method(): B {}
        }
    }
});

$b = new B;

?>
===DONE===
--EXPECT--
===DONE===
