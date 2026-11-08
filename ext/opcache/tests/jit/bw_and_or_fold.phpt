--TEST--
JIT: IR folding of (a op b) op a for BW_AND and BW_OR
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
function or1(int $b) { return $b | ($b | 7); }
function or2(int $b) { return ($b | 7) | $b; }
function and1(int $b) { return $b & ($b & 7); }
function and2(int $b) { return ($b & 7) & $b; }

var_dump(or1(2), or2(2), and1(10), and2(10));
?>
--EXPECT--
int(7)
int(7)
int(2)
int(2)
