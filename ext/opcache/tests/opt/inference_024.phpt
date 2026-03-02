--TEST--
Type inference 024: FETCH_DIM_W
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function foo() {
    for ($cnt = 0; $cnt < 6; $cnt++) {
        $b = new SplFixedArray(1);
        $b[0] = $a;
        $g = &$b[0];
        $b = new SplFixedArray(1);
        $b[0] = $b;
        $g = &$b[0];
    }
}
@foo();
?>
DONE
--EXPECT--
DONE
