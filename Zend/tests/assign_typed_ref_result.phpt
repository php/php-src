--TEST--
Result of assigning to typed reference
--FILE--
<?php

class Test {
    public ?string $prop;
}
function test() {
    $obj = new Test;
    $ref =& $obj->prop;
    var_dump($ref = 0);
}
function test2() {
    $obj = new Test;
    $ary = [];
    $ary[0] =& $obj->prop;
    var_dump($ary[0] = 0);
}
test();
test2();

?>
--EXPECT--
string(1) "0"
string(1) "0"
