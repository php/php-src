--TEST--
Fundamental memory leak test on temporaries
--FILE--
<?php

function ops() {
    throw new Exception();
}

try{
    $x = 1;
    $r = [$x] + ops();
} catch (Exception $e) {
}

?>
==DONE==
--EXPECT--
==DONE==
