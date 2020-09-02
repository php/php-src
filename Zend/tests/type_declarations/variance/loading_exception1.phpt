--TEST--
Exception while loading class -- parent case
--FILE--
<?php

spl_autoload_register(function($class) {
    throw new Exception("Class $class does not exist");
});

// Graceful failure allowed
for ($i = 0; $i < 2; $i++) {
    try {
        class B extends A {
        }
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}

interface I {}

spl_autoload_register(function($class) {
    // Tie up B in a variance obligation.
    class X {
        public function test(): I {}
    }
    class Y extends X {
        public function test(): B {}
    }
}, true, true);

// Fallback to fatal error, as we can't unlink class B anymore.
try {
    class B extends A implements I {
    }
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Class A does not exist
Class A does not exist

Fatal error: During inheritance of B with variance dependencies: Uncaught Exception: Class A does not exist in %s:%d
Stack trace:
#0 %s(%d): {closure}('A')
#1 {main} in %s on line %d
