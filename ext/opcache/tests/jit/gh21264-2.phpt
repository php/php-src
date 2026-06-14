--TEST--
GH-21264: Missing reference unwrap for FE_FETCH_R in JIT
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
--FILE--
<?php

class C {
    public $prop = 0;
}

function test($array) {
    $c = new C;
    foreach ($array as $c->prop) {
        $c->prop++;
    }
}

$element = 0;
$array = [&$element, &$element];
test($array);
test($array);
var_dump($element);

?>
--EXPECT--
int(0)
