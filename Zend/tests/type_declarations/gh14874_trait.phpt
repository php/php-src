--TEST--
GH-14874 (Incorrect lineno for class const variance check with trait constant)
--FILE--
<?php

require __DIR__ . '/gh14874_trait.inc';

interface I {
    public const string X = '';
}

class C implements I {
    use T;
}
?>
--EXPECTF--
Fatal error: Type of C::X must be compatible with I::X of type string in %sgh14874_trait.php on line 9
