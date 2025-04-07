--TEST--
public(set) inner class
--FILE--
<?php

class Outer {
    public(set) class Inner {
    }
}

?>
--EXPECTF--
Fatal error: Cannot use the public(set) modifier on a nested class in %s on line %d
