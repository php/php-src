--TEST--
Interface constant visibility should be invariant
--FILE--
<?php
interface I {
    public const FOO = 'foo';
}

class C implements I {
    private const FOO = 'foo';
}
?>
--EXPECTF--
Fatal error: Access level to C::FOO must be public (as in interface I) in %s on line %d
