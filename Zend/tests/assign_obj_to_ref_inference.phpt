--TEST--
Assigning an object of known type to a reference variable
--FILE--
<?php

class Test {
    public int $x = 42;
}

function test1() {
    $r =& $o;
    $o = new Test;
    $r = new stdClass;
    $r->x = 3.141;
    var_dump(is_float($o->x));
}

function test2($o) {
    $r =& $o;
    if ($o instanceof Test) {
        $r = new stdClass;
        $r->x = 3.141;
        var_dump(is_float($o->x));
    }
}

function test3(Test &$o) {
    $GLOBALS['r'] = new stdClass;
    $GLOBALS['r']->x = 3.141;
    var_dump(is_float($o->x));
}

test1();
test2(new Test);
$r = new Test;
test3($r);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
