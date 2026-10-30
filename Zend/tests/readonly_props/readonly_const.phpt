--TEST--
Class constants cannot be readonly
--FILE--
<?php

class Test {
    readonly const X = 1;
}

?>
--EXPECTF--
Fatal error: Cannot use the readonly modifier on a class constant in %s on line %d
