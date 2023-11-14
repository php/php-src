--TEST--
Bug #81225: Wrong result with pow operator with JIT enabled
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=function
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
function add_with_positive(int $a) {
    $a = $a % 10;
    $b = $a + 1;
    $c = $a + 100;
    $d = $a + 2147483647;          // 0x7fff,ffff
    $e = $a + 2147483648;          // 0x8000,0000     cannot encoded as imm field of lea r1, [r2 + imm]
    $f = $a + 78187493394;         // 0x12,1234,5678  cannot encoded as imm field of lea r1, [r2 + imm]
    var_dump($b, $c, $d, $e, $f);
}

function add_with_negative(int $a) {
    $a = $a % 10;
    $b = $a + (-1);
    $c = $a + (-100);
    $d = $a + (-2147483648);       // 0xFFFF,FFFF,8000,0000
    $e = $a + (-2147483649);       // 0xFFFF,FFFF,7FFF,FFFF  cannot encoded as imm field of lea r1, [r2 + imm]
    $f = $a + (-261458978401740);  // 0xFFFF,1234,5678,1234  cannot encoded as imm field of lea r1, [r2 + imm]
    var_dump($b, $c, $d, $e, $f);
}

function sub_with_positive(int $a) {
    $a = $a % 10;
    $b = $a - 1;
    $c = $a - 100;
    $d = $a - 2147483647;          // 0x7fff,ffff
    $e = $a - 2147483648;          // 0x8000,0000
    $f = $a - 2147483649;          // 0x8000,0001     cannot encoded as imm field of lea r1, [r2 + imm]
    $g = $a - 78187493394;         // 0x12,1234,5678  cannot encoded as imm field of lea r1, [r2 + imm]
    var_dump($b, $c, $d, $e, $f, $g);
}

function sub_with_negative(int $a) {
    $a = $a % 10;
    $b = $a - (-1);
    $c = $a - (-100);
    $d = $a - (-2147483647);       // 0xFFFF,FFFF,8000,0001
    $e = $a - (-2147483648);       // 0xFFFF,FFFF,8000,0000  cannot encoded as imm field of lea r1, [r2 + imm]
    $f = $a - (-2147483649);       // 0xFFFF,FFFF,7FFF,FFFF  cannot encoded as imm field of lea r1, [r2 + imm]
    $g = $a - (-261458978401740);  // 0xFFFF,1234,5678,1234  cannot encoded as imm field of lea r1, [r2 + imm]
    var_dump($b, $c, $d, $e, $f, $g);
}

add_with_positive(2);
add_with_negative(2);
sub_with_positive(2);
sub_with_negative(2);
?>
--EXPECT--
int(3)
int(102)
int(2147483649)
int(2147483650)
int(78187493396)
int(1)
int(-98)
int(-2147483646)
int(-2147483647)
int(-261458978401738)
int(1)
int(-98)
int(-2147483645)
int(-2147483646)
int(-2147483647)
int(-78187493392)
int(3)
int(102)
int(2147483649)
int(2147483650)
int(2147483651)
int(261458978401742)
