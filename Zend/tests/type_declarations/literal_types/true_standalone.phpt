--TEST--
true can be used as a standalone type
--FILE--
<?php

function test(true $v): true {
    return $v;
}

var_dump(test(true));

?>
--EXPECT--
bool(true)
