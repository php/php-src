--TEST--
The ASSIGN_OBJ_OP cache slot is on the OP_DATA opcode
--FILE--
<?php
function test($a) {
    $b = "x";
    $a->$b = 1;
    $a->$b &= 1;
    var_dump($a->$b);
}
test(new stdClass);
?>
--EXPECT--
int(1)
