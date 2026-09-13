--TEST--
GH-22399 (Lazy object double free): zend_object_make_lazy() unset-declared-properties loop (ghost variant)
--DESCRIPTION--
zend_object_make_lazy()'s unset-declared-properties loop had the same
dtor-before-clear bug GH-22401 fixed in zend_lazy_object_revert_init() and
zend_lazy_object_init_proxy(), but was never patched there. A destructor
reentering through the property slot being cleared could free the object
twice. Reproduced under ASAN with USE_ZEND_ALLOC=0.
--FILE--
<?php

class A {
    public $b;
}

class B {
    public function __construct(public $a) {}
    public function __destruct() {
        // Reentrant: fires while zend_object_make_lazy() is still in the
        // middle of tearing down $a's "b" slot below. $this->a is that same
        // $a, and "b" is the very property make_lazy is clearing.
        unset($this->a->b);
    }
}

// $a: object refcount 1 (held by the local variable only).
$a = new A();

// B::__construct() promotes its $a parameter into $this->a, which bumps A's
// refcount to 2 (local variable "$a" + B's "a" property). The resulting B
// instance is then stored into A's "b" property and settles at refcount 1
// (reachable only via $a->b). A and B now form a reference cycle
// ($a->b->a === $a); B has no root reference outside that cycle, so it can
// only go away by dropping A's "b" slot (or via the cycle collector).
$a->b = new B($a);

$reflector = new ReflectionClass(A::class);

// A has no __destruct of its own, so zend_object_make_lazy() skips the
// "call the object's own destructor" step entirely and goes straight to the
// "unset() declared properties" loop (Zend/zend_lazy_objects.c, ~line 328).
// That loop reaches the slot for "b" and does, in order:
//
//   zval garbage;
//   ZVAL_COPY_VALUE(&garbage, p);   // (1) copy B's zval out to a local
//   ZVAL_UNDEF(p);                  // (2) clear A's "b" slot FIRST
//   zval_ptr_dtor(&garbage);        // (3) drop B's refcount 1 -> 0, freeing it
//
// Step (3) is what synchronously invokes B::__destruct(). By that point
// A's "b" slot was already cleared in step (2), so the reentrant
// unset($this->a->b) below sees an UNDEF slot and is a no-op, instead of
// freeing B a second time through a stale reference. Before this fix, the
// dtor ran *before* the slot was cleared, so the reentrant unset() would
// free B while the outer loop was still using it -- a heap-use-after-free.
// This is the same bug class GH-22401 fixed in
// zend_lazy_object_revert_init() and zend_lazy_object_init_proxy(), now
// also applied here.
$reflector->resetAsLazyGhost($a, function ($obj) {
    // Never reached: nothing below triggers initialization of the ghost.
    echo "initializer called\n";
});

var_dump($a);
printf("Is lazy: %d\n", $reflector->isUninitializedLazyObject($a));
echo "done\n";

?>
--EXPECTF--
lazy ghost object(A)#%d (0) {
}
Is lazy: 1
done
