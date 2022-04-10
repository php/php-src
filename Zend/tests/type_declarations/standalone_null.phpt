--TEST--
Null can be used as a standalone type
--FILE--
<?php

function test(null $v): null {
    return $v;
}

var_dump(test(null));

?>
--EXPECT--
NULL
