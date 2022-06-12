--TEST--
False can be used as a standalone type
--FILE--
<?php

function test(false $v): false {
    return $v;
}

var_dump(test(false));

?>
--EXPECT--
bool(false)
