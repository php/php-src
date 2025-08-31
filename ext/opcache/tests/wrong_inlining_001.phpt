--TEST--
Pass result of inlined function by reference
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
    foo(get_const());
}

if (true) {
    function foo(&$ref) {}
}

test();
?>
OK
--EXPECTF--
Notice: Only variables should be passed by reference in %swrong_inlining_001.php on line 7
OK
