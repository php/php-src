--TEST--
exit() and finally (3)
--FILE--
<?php

// TODO: In the future, we should execute the finally block.

function test() {
    try {
        exit("Exit\n");
    } finally {
        return 42;
    }
}
var_dump(test());

?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
Exit