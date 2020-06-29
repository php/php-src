--TEST--
exit() and finally (3)
--FILE--
<?php

function test() {
    try {
        exit("Exit\n");
    } finally {
        return 42;
    }
}
var_dump(test());

?>
--EXPECT--
int(42)
