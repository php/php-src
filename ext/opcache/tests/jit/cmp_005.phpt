--TEST--
JIT CMP: 005 Comparisons with immediate values
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
function foo($a) {
    $b = 0;
    $c = 31;
    $d = 0xfff;
    $e = 0x1000;
    $f = 0xfff000;
    $g = 0xff001;          // Cannot be encoded into imm12 field
    $h = 0x1000000;        // Cannot be encoded into imm12 field
    $i = 0xf12345678;      // Cannot be encoded into imm12 field

    var_dump($a > $b ? 1 : 0);
    var_dump($a > $c ? 1 : 0);
    var_dump($a > $d ? 1 : 0);
    var_dump($a > $e ? 1 : 0);
    var_dump($a > $f ? 1 : 0);
    var_dump($a > $g ? 1 : 0);
    var_dump($a > $h ? 1 : 0);
    var_dump($a > $i ? 1 : 0);
}

function bar($a) {
    $b = 0;
    $c = -31;
    $d = -4095;            // negation of 0xfff
    $e = -4096;            // negation of 0x1000
    $f = -16773120;        // negation of 0xfff000
    $g = -1044481;         // negation of 0xff001
    $h = -16777216;        // negation of 0x1000000
    $i = -64729929336;     // negation of 0xf12345678

    var_dump($a > $b ? 1 : 0);
    var_dump($a > $c ? 1 : 0);
    var_dump($a > $d ? 1 : 0);
    var_dump($a > $e ? 1 : 0);
    var_dump($a > $f ? 1 : 0);
    var_dump($a > $g ? 1 : 0);
    var_dump($a > $h ? 1 : 0);
    var_dump($a > $i ? 1 : 0);
}

foo(42);
bar(42);
?>
--EXPECT--
int(1)
int(1)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
