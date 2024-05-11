--TEST--
JIT ASSIGN: 011
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
    $a = 1;
    $c = 2;
    $d = 3;
    if ($a) {
        $b = array();
    } else {
        $b =& $c;
    }
    $b = $d;
    var_dump($b, $c);
}
foo();
?>
--EXPECT--
int(3)
int(2)
