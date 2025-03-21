--TEST--
static resolution
--FILE--
<?php

class Outer {
    class Inner {}

    public function test() {
        return new static:>Inner();
    }
}
var_dump(new Outer()->test());
?>
--EXPECTF--
Fatal error: Cannot use the static modifier on an inner class in %s on line %d
