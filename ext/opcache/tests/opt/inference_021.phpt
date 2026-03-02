--TEST--
Type inference 021;
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function foo() {
    for(;;){
        $b->y++;
        $b *= $a;
        $a++;
    }
}
?>
DONE
--EXPECT--
DONE
