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
Parse error: syntax error, unexpected identifier "int", expecting "class" in %s on line %d
