--TEST--
GH-18026: Confusing "amp" reference in parser error
--FILE--
<?php

class Demo {
    private (set) mixed $v1;
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token ")", expecting token "&" in %s on line %d
