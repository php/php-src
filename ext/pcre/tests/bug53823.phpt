--TEST--
Bug #53823 - preg_replace: * qualifier on unicode replace garbles the string
--FILE--
<?php
var_dump(preg_replace('/[^\pL\pM]*/iu', '', 'áéíóú'));
// invalid UTF-8
var_dump(preg_replace('/[^\pL\pM]*/iu', '', "\xFCáéíóú"));
var_dump(preg_replace('/[^\pL\pM]*/iu', '', "áéíóú\xFC"));
?>
--EXPECT--
string(10) "áéíóú"
NULL
NULL
