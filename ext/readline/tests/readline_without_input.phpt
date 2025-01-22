--TEST--
readline() function - without input
--CREDITS--
Jonathan Stevens <info at jonathanstevens dot be>
User Group: PHP-WVL & PHPGent #PHPTestFest
--EXTENSIONS--
readline
--FILE--
<?php
var_dump(readline());
var_dump(readline('Prompt:'));
?>
--EXPECTF--
bool(false)
%Abool(false)
