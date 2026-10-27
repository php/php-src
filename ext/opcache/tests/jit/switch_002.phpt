--TEST--
JIT SWITCH_STRING on a reference holding a non-string value
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_hot_loop=1
--EXTENSIONS--
opcache
--FILE--
<?php
function test($a) {
    $n = 0;
    foreach ($a as &$x) {
        switch ($x) {
            case "a": case "b": $n++; break;
        }
    }
    return $n;
}
var_dump(test(["a"]));
var_dump(test([1]));
?>
--EXPECT--
int(1)
int(0)
