--TEST--
Class order allowed with autoloading (2)
--FILE--
<?php

spl_autoload_register(function($class) {
    if ($class === 'A') {
        class A {
            public function method() : B {}
        }
    } else if ($class == 'B') {
        class B extends A {
            public function method() : C {}
        }
    } else {
        class C extends B {
        }
    }
});

// Same as autoload1 test case, but with a different autoloading root
$b = new B;

?>
===DONE===
--EXPECT--
===DONE===
