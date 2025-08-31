--TEST--
Type inference 013: ASSIGN_DIM
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function y(){
    for(;;){
        $x[] &= y;
        $x = false;
        $x[""] = y;
    }
}
?>
DONE
--EXPECT--
DONE
