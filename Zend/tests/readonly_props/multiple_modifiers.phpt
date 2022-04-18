--TEST--
Cannot use multiple readonly modifiers
--FILE--
<?php

class Test {
    readonly readonly int $prop;
}

?>
--EXPECTF--
Fatal error: Multiple readonly modifiers are not allowed in %s on line %d
