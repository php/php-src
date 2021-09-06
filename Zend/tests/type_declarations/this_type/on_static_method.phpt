--TEST--
$this on static method
--FILE--
<?php

class Test {
    public static function method(): $this {
    }
}

?>
--EXPECTF--
Fatal error: Cannot use "$this" type on static method in %s on line %d
