--TEST--
Class order allowed with autoloading (6)
--FILE--
<?php

// Similar to variance3, but one more class hierarchy in the cycle
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
            public function method(): Q {}
        }
    } else if ($class == 'Y') {
        class Y extends X {
            public function method(): R {}
        }
    } else if ($class == 'Q') {
        var_dump(class_exists('A'));
        var_dump(class_exists('B'));
        var_dump(class_exists('X'));
        var_dump(class_exists('Y'));
        class Q {
            public function method(): A {}
        }
    } else if ($class == 'R') {
        class R extends Q {
            public function method(): B {}
        }
    }
});

$b = new B;

?>
===DONE===
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
===DONE===
