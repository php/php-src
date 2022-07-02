--TEST--
SCCP 018: Object assignment
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
;opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $a = new stdClass;
    $b = $a;
    $a->x = 5;
    $b->x = 42;
    echo $a->x;
    echo "\n";
}
foo();
?>
--EXPECT--
42
