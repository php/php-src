--TEST--
JIT ADD: 007 Addition with immediate values
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
    $j = -31;              // Cannot be encoded into imm12 field

    $a = $a + $b;
    $a = $a + $c;
    $a = $a + $d;
    $a = $a + $e;
    $a = $a + $f;
    $a = $a + $g;
    $a = $a + $h;
    $a = $a + $i;
    $a = $a + $j;
    var_dump($a);
}

function bar($a) {
    $b = 0;
    $c = 31;
    $d = 0xfff;
    $e = 0x1000;
    $f = 0xfff000;
    $g = 0xff001;          // Cannot be encoded into imm12 field
    $h = 0x1000000;        // Cannot be encoded into imm12 field
    $i = 0xf12345678;      // Cannot be encoded into imm12 field
    $j = -31;              // Cannot be encoded into imm12 field

    $a = $a - $b;
    $a = $a - $c;
    $a = $a - $d;
    $a = $a - $e;
    $a = $a - $f;
    $a = $a - $g;
    $a = $a - $h;
    $a = $a - $i;
    $a = $a - $j;
    var_dump($a);
}

foo(42);
bar(0x1f12345678);
?>
--EXPECT--
int(64764532386)
int(68684873728)
