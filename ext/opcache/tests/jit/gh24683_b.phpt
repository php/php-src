--TEST--
GH-24683 (B): tracing JIT must preserve overwritten runtime CV values
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
// php -d opcache.enable_cli=1 -d opcache.jit=tracing -d opcache.jit_buffer_size=64M repro.php
//
// Case 1 stores to $ram and moves on to case 2. Case 2 sets $x to a value it
// only knows at run time (0x800E0000 | $zero, where $zero is 0), uses it as an
// address, and overwrites $x with the byte loaded from there (4). Once the loop
// trace is compiled, $x sometimes comes back as 0x800E0000 instead.

final class Bus
{
    public function read8(int $address): int { return 0; }
}

$fn = static function (Bus $bus, array &$r, array &$ram, array &$sp, int $pc): int {
    $value = $r[4];
    $base = $r[15];
    $zero = $r[0];
    while (true) {
        switch ($pc) {
            case 1:
                $a = ($base + 2) & 0xFFFFFFFF;
                {
                    $p = $a & 0x1FFFFFFF;
                    $inRam = $a < 0xC0000000 && $p < 0x00800000;
                    if ($inRam) { $k = ($p & 0x1FFFFF) >> 2; $ram[$k] = ($ram[$k] & ~(65535 << (($p & 3) << 3))) | (($value & 65535) << (($p & 3) << 3)); }
                }
                $pc = 2;
                continue 2;
            case 2:
                $x = 0x800E0000 | $zero;
                $a = ($x + 0xFFFFEDED) & 0xFFFFFFFF;
                $p = $a & 0x1FFFFFFF;
                $byte = ((($inRam ? ($ram[($p & 0x1FFFFF) >> 2] >> (($p & 3) << 3)) & 0xFF : ($inSp ? ($sp[($p - 0x1F800000) >> 2] >> (($p & 3) << 3)) & 0xFF : $bus->read8($a))) ^ 0x80) - 0x80) & 0xFFFFFFFF;
                $x = $byte;
                $pc = 3;
                continue 2;
            default:
                goto leave;
        }
    }
    leave:
    $r[12] = $x;
    return $pc;
};

$bus = new Bus();
$ram = array_fill(0, 0x80000, 0);
$ram[0xDEDED >> 2] = 0x0003047F; // the byte at 0xDEDED is 4
$sp = [];
$wrong = 0;
for ($k = 0; $k < 300; $k++) {
    $r = array_fill(0, 32, 0);
    $r[4] = 2;
    $fn($bus, $r, $ram, $sp, 1);
    if ($r[12] !== 4) {
        if ($wrong++ === 0) {
            printf("call %d: \$x is 0x%X, expected 4\n", $k, $r[12]);
        }
    }
}
echo "wrong: $wrong\n";
--EXPECT--
wrong: 0
