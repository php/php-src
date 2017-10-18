--TEST--
readline(): Basic test
--CREDITS--
Sergii Ivashchenko <serg.ivashchenko@gmail.com>
Paras Sood <paras.sood@outlook.com>
# PHPDublin PHPTestFest 2017
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip"); ?>
--FILE--
<?php
var_dump(readline('What is your name?'));
?>
--STDIN--
Paras
--EXPECT--
What is your name?Paras
string(5) "Paras"