--TEST--
GH-24389 (Tracing JIT: type guard on entry of an inlined call exits with an uninitialized opline)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
function right($x, $y) {
    $a = $x + $y;
    return $x - $y;
}
function driver($x, $y) {
    $xr = &$x;
    $yr = &$y;
    for ($i = 0; $i < 300; $i++) {
        $r = right($x, $y);
    }
    return $r;
}
var_dump(driver(3, 2));
var_dump(driver(3.5, 2.5));
?>
--EXPECT--
int(1)
float(1)
