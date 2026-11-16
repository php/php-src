--TEST--
JIT SWITCH_STRING on a reference (function JIT)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
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
var_dump(test(["a", "b", "c"]));
var_dump(test([1]));
?>
--EXPECT--
int(2)
int(0)
