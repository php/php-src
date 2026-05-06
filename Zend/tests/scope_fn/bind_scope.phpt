--TEST--
Scope function closure: scope rebind to a compatible scope mutates in place
--FILE--
<?php
class A {
    public int $secret = 42;
}
class B {
    public int $secret = 99;
}

function test() {
    /* Closure with no $this: scope rebind allowed. The body never reads
     * $this; we just verify the bind returns the same object and updates
     * the visible scope (via ReflectionFunction). */
    $fn = fn() { return 1; };
    $bound = Closure::bind($fn, null, A::class);
    var_dump($bound === $fn);   // mutate-in-place: same object
    $r = new ReflectionFunction($bound);
    var_dump($r->getClosureScopeClass()?->name);

    /* Rebind to incompatible internal class fails (warning today, error in PHP 9). */
    try {
        $r = Closure::bind($fn, null, ArrayObject::class);
        var_dump($r);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
test();
?>
--EXPECTF--
bool(true)
string(1) "A"

Warning: Cannot bind closure to scope of internal class ArrayObject, this will be an error in PHP 9 in %s on line %d
NULL
