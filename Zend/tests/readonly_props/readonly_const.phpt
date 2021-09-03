--TEST--
Class constants cannot be readonly
--FILE--
<?php

class Test {
    readonly const X = 1;
}

?>
--EXPECTF--
Fatal error: Cannot use 'readonly' as constant modifier in %s on line %d
