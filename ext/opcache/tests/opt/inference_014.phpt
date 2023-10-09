--TEST--
Type inference 014: ASSIGN_DIM_OP
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function y() {
    for(;;){
        $x[y] &= y;
        $x = false;
        $x[""]=y;
    }
}
?>
DONE
--EXPECT--
DONE
