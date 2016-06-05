--TEST--
Incorrect NOP removal on jump to NOP
--FILE--
<?php

function test(int $i) : int {
    if ($i == 1) {
        $x = $i + 1;
    }
    return $i;
}
var_dump(test(42));

?>
--EXPECT--
int(42)
