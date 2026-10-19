--TEST--
GH-24391 (JIT: switch on constant of wrong type takes default instead of falling back to the case chain)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.optimization_level=0
--FILE--
<?php
function long_switch() {
    switch (null) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
        case 4: return 5;
        default: return 6;
    }
}

function string_switch() {
    switch (0) {
        case "aa": return 1;
        case "bb": return 2;
        case "cc": return 3;
        default: return 6;
    }
}

var_dump(long_switch());
var_dump(string_switch());
?>
--EXPECT--
int(1)
int(6)
