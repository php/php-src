--TEST--
Type inference for $ary[$idx]->prop +=
--FILE--
<?php

function test() {
    $ary = [];
    $ary[0]->y += 2;
    var_dump(is_object($ary[0]));
}
test();

?>
--EXPECTF--
Notice: Undefined offset: 0 in %s on line %d

Warning: Creating default object from empty value in %s on line %d

Notice: Undefined property: stdClass::$y in %s on line %d
bool(true)
