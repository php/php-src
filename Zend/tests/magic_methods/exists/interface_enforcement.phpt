--TEST--
__exists: interface contract is enforced like any other method (missing implementation fatals)
--FILE--
<?php

/* When an interface declares __exists, the implementing class must
 * provide a compatible implementation, exactly like any other method.
 * Omitting it produces the standard "must implement" compile-time
 * fatal: the engine attaches no special-case to __exists here. */

interface I {
    public function __exists(string $n): bool;
}

class C implements I {
}

?>
--EXPECTF--
Fatal error: Class C contains 1 abstract method and must therefore be declared abstract or implement the remaining method (I::__exists) in %s on line %d
