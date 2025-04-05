--TEST--
invalid inner class
--FILE--
<?php

class Outer {
    int class Inner {
    }
}

?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "int", expecting "class" or "enum" in %s on line %d
