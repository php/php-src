--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): global by ref
--FILE--
<?php

class Test {
    function __destruct(){
        unset($GLOBALS['a']);
    }
}

function &returnsVal() {
    $a = 42;
    return $a;
}
$a = new Test;
var_dump($a =& returnsVal());

?>
--EXPECT--
int(42)
