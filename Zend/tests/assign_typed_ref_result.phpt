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
test();

?>
--EXPECT--
string(1) "0"
