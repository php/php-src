--TEST--
Lazy objects: clone does not assert when __clone() declares a function causing a fatal error
--FILE--
<?php

// Regression test for GH-20905:
// When __clone() declares a function that was already declared (triggering E_ERROR
// and bailout), del_info() was called during shutdown on an object whose info entry
// had already been removed, causing an assertion failure in debug builds.

function f() {}

class A {
    public stdClass $p;

    public function __construct()
    {
        $this->p = new stdClass;
    }

    public function __clone()
    {
        // Redeclaring an already-declared function triggers E_ERROR -> bailout.
        // The lazy object machinery must not assert during the resulting shutdown.
        function f() {}
    }
}

$r = new ReflectionClass(A::class);
clone $r->newLazyProxy(fn() => new A);
?>
--EXPECTF--
Fatal error: Cannot redeclare function f() (previously declared in %s:%d) in %s on line %d
