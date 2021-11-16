--TEST--
Range inference should not hang
--FILE--
<?php

function test() {
    $a = 0;
    while (true) {
        $a = $a+!$a=$a/!!~$a;
    }
}

?>
===DONE===
--EXPECT--
===DONE===
