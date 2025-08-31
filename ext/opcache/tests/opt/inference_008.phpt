--TEST--
Type inference 008: ASSIGN_DIM with invalid op1
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function y() {
    $j = 0;
    for(;;) {
        $cs = $a + $a;
        $a = [] ?? $cs[] = $j;
    }
}
?>
DONE
--EXPECT--
DONE
