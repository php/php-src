--TEST--
Bug #69267: mb_strtolower fails on titlecase characters
--FILE--
<?php
$str = "ǅǈǋǲ";
var_dump(mb_strtolower($str));
?>
--EXPECT--
string(8) "ǆǉǌǳ"
