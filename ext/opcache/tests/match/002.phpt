--TEST--
Test match jump table optimizer
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
zend_test.observer.enabled=0
--EXTENSIONS--
opcache
--FILE--
<?php

function test() {
    $x = '2';
    echo match($x) {
        1, 2, 3, 4, 5 => throw new RuntimeException(),
        default => "No match\n",
    };
}
test();

function test2() {
    $x = 2;
    echo match($x) {
        '1', '2', '3', '4', '5' => throw new RuntimeException(),
        default => "No match\n",
    };
}
test2();

?>
--EXPECTF--
$_main:
     ; (lines=6, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 0 %d string("test")
0001 DO_UCALL
0002 INIT_FCALL 0 %d string("test2")
0003 DO_UCALL
0004 DEFER_RUN
0005 RETURN int(1)

test:
     ; (lines=3, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 ECHO string("No match\n")
0001 DEFER_RUN
0002 RETURN null

test2:
     ; (lines=3, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 ECHO string("No match\n")
0001 DEFER_RUN
0002 RETURN null
No match
No match
