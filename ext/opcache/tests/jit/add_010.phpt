--TEST--
JIT ADD: 010 overflow handling
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
function foo($a) {
    var_dump($a+$a=$a+$a=$a+$a=$a);
}
foo(PHP_INT_MAX);
?>
--EXPECT--
float(7.378697629483821E+19)
