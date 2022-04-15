--TEST--
Assigning an object of known type to a reference variable
--FILE--
<?php

class Test {
    public int $x = 42;
}

function test() {
    $r =& $o;
    $o = new Test;
    $r = new stdClass;
    $r->x = 3.141;
    var_dump(is_float($o->x));
}
test();

?>
--EXPECT--
bool(true)
