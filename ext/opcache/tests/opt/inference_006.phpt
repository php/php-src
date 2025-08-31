--TEST--
Type inference 006: FETCH_DIM_W with invalid key type
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function y() {
    $obj=new y;
    u($y[$obj]);
}
?>
DONE
--EXPECT--
DONE
