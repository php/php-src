--TEST--
Don't replace IN_ARRAY result type if the using opcode doesn't support it
--FILE--
<?php

function test($v) {
    $ary = ['x', 'y'];
    var_dump(in_array($v, $ary));
}
test('x');

?>
--EXPECT--
bool(true)
