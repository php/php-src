--TEST--
Incorrect DCE with FE_FETCH
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

function test() {
    $a = ["3"];
    $x = 1;
    foreach ($a as $x) {
        $x = 2.0;
    }
    var_dump($x);
}
test();

?>
--EXPECT--
float(2)
