--TEST--
Fundamental memory leak test on temporaries
--XFAIL--
See Bug #62210 and attempt to fix it in "tmp_livelibess" branch
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
