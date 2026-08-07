--TEST--
Bug #70089 (segfault in PHP 7 at ZEND_FETCH_DIM_W_SPEC_VAR_CONST_HANDLER ())
--FILE--
<?php
function dummy($a) {
}

try {
    chr(0)[0][] = 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unset(chr(0)[0][0]);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
eval("function runtimetest(&\$a) {} ");
try {
    runtimetest(chr(0)[0]);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    ++chr(0)[0];
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Cannot use string offset as an array
Error: Cannot use string offset as an array
Error: Cannot create references to/from string offsets
Error: Cannot increment/decrement string offsets
