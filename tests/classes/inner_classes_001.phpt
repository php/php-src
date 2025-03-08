--TEST--
multiple access modifiers
--FILE--
<?php

class Outer {
    public private class Inner {
    }
}

?>
--EXPECTF--
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
