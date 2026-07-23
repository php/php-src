--TEST--
GH-21593: Function JIT JMPNZ smart branch
--CREDITS--
paulmhh
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
--FILE--
<?php

function test1($a) {
    if (isset($a?->a)) {
        echo "1\n";
    }
}

function test2($a) {
    if (!isset($a?->a)) {
        echo "2\n";
    }
}

function test3($a) {
    if (empty($a?->a)) {
        echo "3\n";
    }
}

function test4($a) {
    if (!empty($a?->a)) {
        echo "4\n";
    }
}

$a = new stdClass;
$a->a = 'a';

test1($a);
test2($a);
test3($a);
test4($a);

?>
--EXPECT--
1
4
