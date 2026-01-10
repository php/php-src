--TEST--
Method cannot be readonly
--FILE--
<?php

class Test {
    readonly function test() {}
}

?>
--EXPECTF--
Fatal error: Cannot use the readonly modifier on a method in %s on line %d
