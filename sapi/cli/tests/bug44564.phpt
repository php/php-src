--TEST--
Bug #44564 (escapeshellarg removes UTF-8 multi-byte characters)
--FILE--
<?php

var_dump(escapeshellcmd('f{o}<€>'));
var_dump(escapeshellarg('f~|;*Þ?'));
var_dump(escapeshellcmd('?€®đæ?'));
var_dump(escapeshellarg('aŊł€'));

?>
--EXPECT--
string(13) "f\{o\}\<€\>"
string(10) "'f~|;*Þ?'"
string(13) "\?€®đæ\?"
string(10) "'aŊł€'"
