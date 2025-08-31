--TEST--
Type inference 009: FRTCH_DIM_W
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function y() {
    for(;;) {
        $arr[y][]=y;
        $arr=[''=>y];
    }
}
?>
DONE
--EXPECT--
DONE
