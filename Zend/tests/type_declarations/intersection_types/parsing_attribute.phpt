--TEST--
Intersection type parsing interaction with attributes
--FILE--
<?php

class Test {
    public X& #[Comment]
        Z $p4;
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "#[" in %s on line %d
