--TEST--
GH-24396: Assertion failure in zend_jit_def_reg() "reg forwarding" with chained Pi nodes
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
function f() {
    $i = 0;
    do {
        $i = ($i + 1) & 255;
    } while ($i < 2 || $i > 200);
    return $i;
}
var_dump(f());
?>
--EXPECT--
int(2)
