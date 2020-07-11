--TEST--
Constructor promotion only permits visibility modifiers
--FILE--
<?php

class Test {
    public function __construct(public static $x) {}
}

?>
--EXPECTF--
Parse error: syntax error, unexpected 'static' (T_STATIC), expecting variable (T_VARIABLE) in %s on line %d
