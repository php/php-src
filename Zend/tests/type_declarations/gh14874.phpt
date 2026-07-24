--TEST--
GH-14874 (Incorrect lineno for property and class const variance check)
--FILE--
<?php

class C extends P {
    public const int X = 42;
}

class P {
    public const string X = 'X';
}

?>
--EXPECTF--
Fatal error: Type of C::X must be compatible with P::X of type string in %s on line 4
