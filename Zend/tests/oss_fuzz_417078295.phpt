--TEST--
OSS-Fuzz #417078295
--FILE--
<?php

function foo() {
    $a = new stdClass();
    static $a = $a;
    debug_zval_dump($a);
}

foo();

?>
--EXPECT--
object(stdClass)#1 (0) refcount(2){
}
