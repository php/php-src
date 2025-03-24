--TEST--
static access modifiers
--FILE--
<?php

class Outer {
    static class Inner {
    }
}

?>
--EXPECTF--
Fatal error: Cannot use the static modifier on a inner class in %s on line %d
