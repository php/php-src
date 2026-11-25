--TEST--
JIT ASSIGN_DIM: result in double register with multiple assignment paths
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
--FILE--
<?php

function f($k) {
    $x = [];
    for ($i = 0; $i < 3; $i++) {
        $a + $x[] = $k;
        $a = $b = $a = $x[$k . $k] = 0.5;
    }
    var_dump($a, $b);
}

f("1");

?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d
float(0.5)
float(0.5)
