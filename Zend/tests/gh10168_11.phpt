--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): global by ref val error
--FILE--
<?php

class Test {
    function __destruct(){
        unset($GLOBALS['a']);
    }
}

function returnsVal() {
    return 42;
}
$a = new Test;
var_dump($a =& returnsVal());

?>
--EXPECTF--
Notice: Only variables should be assigned by reference in %s on line %d
int(42)
