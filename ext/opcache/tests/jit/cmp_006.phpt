--TEST--
JIT CMP: 006 Undefined variable checks
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
function test1($c) {
    if ($c) {
        $x = 1;
    }
    var_dump($x == 1);
}
function test2($c) {
    if ($c) {
        $x = 1.0;
    }
    var_dump($x == 1.0);
}
function test3($c) {
    if (!$c) {
        $x = 1;
    }
    if ($c) {
        $y = 1;
    }
    var_dump($x == $y);
}
function test4($c) {
    if (!$c) {
        $x = 1.0;
    }
    if ($c) {
        $y = 1.0;
    }
    var_dump($x == $y);
}
test1(false);
test2(false);
test3(false);
test4(false);
?>
--EXPECTF--
Warning: Undefined variable $x in %s on line %d
bool(false)

Warning: Undefined variable $x in %s on line %d
bool(false)

Warning: Undefined variable $y in %s on line %d
bool(false)

Warning: Undefined variable $y in %s on line %d
bool(false)
