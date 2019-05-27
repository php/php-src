--TEST--
Class order allowed with autoloading (1)
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

$c = new C;

?>
===DONE===
--EXPECT--
===DONE===
