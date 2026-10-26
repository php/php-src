--TEST--
GH-24683 (CSE result outlives its CV spill slot)
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die('skip requires 64-bit integers'); ?>
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
$fn = require __DIR__ . '/gh24683_cse.inc';
$wrong = 0;
for ($i = 0; $i < 1000; $i++) {
    $ram = array_fill(0, 0x80000, 0);
    $ram[0x108 >> 2] = 0xA000E1EC;
    $regs = array_fill(0, 32, 0);
    $regs[29] = 0x85D8;
    $regs[31] = 0xE10;
    if ($fn($regs, $ram, 58) !== 0xA000E1EC) { $wrong++; }
}
echo "$wrong of 1000 calls wrong\n";
--EXPECT--
0 of 1000 calls wrong
