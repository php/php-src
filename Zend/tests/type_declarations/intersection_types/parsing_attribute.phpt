--TEST--
Intersection type parsing and by-ref parsing interaction with attributes
--FILE--
<?php

class Test {
    public X& #[Comment]
        Z $p;
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "#[" in %s on line %d
