--TEST--
Type inference 011: FETCH_DIM_FUNC_ARG
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function() {
    for(; $a++; $a=y+6) {
        y($a[y]);
    }
}
?>
DONE
--EXPECTF--
DONE
