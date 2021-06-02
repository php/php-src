--TEST--
Method cannot be readonly
--FILE--
<?php

class Test {
    readonly function test() {}
}

?>
--EXPECTF--
Fatal error: Cannot use 'readonly' as method modifier in %s on line %d
