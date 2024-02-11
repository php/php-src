--TEST--
Type inference 004: Type narrowing warning during type inference of ZEND_FETCH_DIM_W
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function y() {
    for(;;){
        $arr[]->y = c;
        $arr = c;
    }
}
?>
DONE
--EXPECTF--
Fatal error: Cannot use [] for reading in %s on line %d
