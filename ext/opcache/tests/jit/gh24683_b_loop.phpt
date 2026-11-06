--TEST--
GH-24683 (B): loop guards must observe the incoming CV value
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=61
opcache.jit_hot_func=127
opcache.jit_hot_return=8
opcache.jit_hot_side_exit=8
opcache.file_update_protection=0
--FILE--
<?php
function test(array $r, int $pc) {
    $zero = $r[0];
    $x = $r[2];
    while (true) {
        switch ($pc) {
            case 2:
                $x = 0x800E0000 | $zero;
                $a = ($x + 0xFFFFEDED) & 0xFFFFFFFF;
                $p = $a & 0x1FFFFFFF;
                $byte = ((($r[1] >> (($p & 3) << 3)) & 0xFF) ^ 0x80) - 0x80;
                $x = $byte;
                $pc = 3;
                continue 2;
            default: goto leave;
        }
    }
    leave: return $x;
}
$wrong = 0;
for ($i = 0; $i < 1000; $i++) {
    $r = [0, 0x0003047F, 4];
    $pc = $i < 500 ? 2 : 3;
    if (test($r, $pc) !== 4) {
        $wrong++;
    }
}
echo "wrong: $wrong\n";
--EXPECT--
wrong: 0
