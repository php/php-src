--TEST--
JIT ASSIGN: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
;opcache.jit_debug=1
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $ref1 = 2.3;
    $ref2 =& $ref1;
    $a = array();
    $b = $a;
    $c = $a;
    $a = $ref1;
    $x = $a;
    var_dump($x, $b, $c);
}
foo();
?>
--EXPECT--
float(2.3)
array(0) {
}
array(0) {
}
