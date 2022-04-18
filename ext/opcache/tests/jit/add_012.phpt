--TEST--
JIT ADD: 012 register allocation for 64-bit constant
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
$x = 0;
$y = [0]; 
$y[$x]++;
$y[$x] += 4467793343;
?>
DONE
--EXPECT--
DONE