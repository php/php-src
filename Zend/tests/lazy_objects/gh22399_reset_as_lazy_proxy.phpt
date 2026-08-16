--TEST--
GH-22399 (Lazy object double free): zend_object_make_lazy() unset-declared-properties loop (proxy variant)
--DESCRIPTION--
Same root cause as gh22399_reset_as_lazy_ghost.phpt, exercised through
ReflectionClass::resetAsLazyProxy() instead of resetAsLazyGhost().
--FILE--
<?php

class A {
    public $b;
}

class B {
    public function __construct(public $a) {}
    public function __destruct() {
        // Reentrant: fires while zend_object_make_lazy() is still in the
        // middle of tearing down $a's "b" slot below.
        unset($this->a->b);
    }
}

// $a: refcount 1 (local variable only).
$a = new A();

// Same cycle as the ghost variant: B::__construct() promotes $a into
// $this->a (A's refcount -> 2: local variable + B's "a" property), then the
// new B settles into $a->b at refcount 1 (reachable only via $a->b). B is
// kept alive purely by this A<->B cycle.
$a->b = new B($a);

$reflector = new ReflectionClass(A::class);

// resetAsLazyProxy() also funnels into zend_object_make_lazy() (same
// function as resetAsLazyGhost(), just with the PROXY strategy flag set).
// Before that flag matters at all, the function runs the shared "unset()
// declared properties" loop against $a's existing state:
//
//   zval garbage;
//   ZVAL_COPY_VALUE(&garbage, p);   // (1) copy B's zval out to a local
//   ZVAL_UNDEF(p);                  // (2) clear A's "b" slot FIRST
//   zval_ptr_dtor(&garbage);        // (3) drop B's refcount 1 -> 0, freeing it
//
// Step (3) synchronously invokes B::__destruct(), but A's "b" slot is
// already UNDEF (step 2) by then, so the reentrant unset($this->a->b)
// below is a no-op instead of a second, invalid free of B. See
// gh22399_reset_as_lazy_ghost.phpt for the full step-by-step of why the
// ordering matters.
//
// Only once this loop finishes does make_lazy get to the proxy-specific
// setup (OBJ_EXTRA_FLAGS(obj) |= IS_OBJ_LAZY_PROXY, storing the factory
// closure for later) -- the loop's safety does not depend on which
// strategy triggered it.
$reflector->resetAsLazyProxy($a, function ($obj) {
    // Never reached: nothing below triggers initialization of the proxy.
    echo "factory called\n";
    return new A();
});

var_dump($a);
printf("Is lazy: %d\n", $reflector->isUninitializedLazyObject($a));
echo "done\n";

?>
--EXPECTF--
lazy proxy object(A)#%d (0) {
}
Is lazy: 1
done
