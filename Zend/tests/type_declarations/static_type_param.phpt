--TEST--
Static type is not allowed in parameters
--FILE--
<?php

class Test {
    public function test(static $param) {
    }
}

?>
--EXPECTF--
Fatal error: Cannot use the static modifier on a parameter in %s on line %d
