--TEST--
Type inference 007: Incorrect array key type inference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function y() {
    for(;;) {
        s($array[]);
        $array = array(""=>"");
    }
}
?>
DONE
--EXPECT--
DONE
