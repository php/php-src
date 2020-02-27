--TEST--
func_get_args with no args
--FILE--
<?php

function foo()
{
    var_dump(func_get_args());
}
foo();

?>
--EXPECT--
array(0) {
}
