--TEST--
readline() function - without input
--CREDITS--
Jonathan Stevens <info at jonathanstevens dot be>
User Group: PHP-WVL & PHPGent #PHPTestFest
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline') || die("skip"); ?>
--FILE--
<?php
var_dump(readline());
var_dump(readline('Prompt:'));
?>
--EXPECTF--
bool(false)
%Abool(false)
