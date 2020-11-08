--TEST--
Constructor promotion only permits visibility modifiers
--FILE--
<?php

class Test {
    public function __construct(public static $x) {}
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "static", expecting variable in %s on line %d
