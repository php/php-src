--TEST--
Exceptions thrown into a rope must not leak
--FILE--
<?php

class Obj {
    function __get($x) {
        throw new Exception();
    }
}

try {
    $x = new Obj;
    $y = 0;
    $r = "$y|$x->x|";
    echo "should never be reached";
} catch (Exception $e) {
    echo "$e\n";
}

try {
    $x = new Obj;
    $y = 0;
    $r = "$y$x->x|";
    echo "should never be reached";
} catch (Exception $e) {
    echo "$e\n";
}

try {
    $x = new Obj;
    $y = 0;
    $r = "$y|$y$x->x";
    echo "should never be reached";
} catch (Exception $e) {
    echo "$e\n";
}

?>
--EXPECTF--
Exception in %s:%d
Stack trace:
#0 %s(%d): Obj->__get('x')
#1 {main}
Exception in %s:%d
Stack trace:
#0 %s(%d): Obj->__get('x')
#1 {main}
Exception in %s:%d
Stack trace:
#0 %s(%d): Obj->__get('x')
#1 {main}
