--TEST--
protected(set) inner class
--FILE--
<?php

class Outer {
    protected(set) class Inner {
    }
}

?>
--EXPECTF--
Fatal error: Cannot use the protected(set) modifier on a nested class in %s on line %d
