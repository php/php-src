--TEST--
JIT ASSIGN: 006
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $a = array();
    $c = array();
    $a = 1;
    $c = $a;
    return $c;
}
var_dump(foo());
?>
--EXPECT--
int(1)
