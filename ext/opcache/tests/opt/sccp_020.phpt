--TEST--
SCCP 020: Object assignment
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
    $b = $a = new stdClass;
    $a->x = 5;
    $b->x = 42;
    echo $a->x;
    echo "\n";
}
foo();
?>
--EXPECT--
42
