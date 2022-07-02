--TEST--
foo($bar) with undefined $bar
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function get_const() {
    return 42;
}

function test() {
    var_dump(get_const($undef));
}

test();
?>
--EXPECTF--
Warning: Undefined variable $undef in %s on line %d
int(42)
