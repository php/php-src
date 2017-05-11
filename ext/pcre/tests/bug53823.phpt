--TEST--
Bug #53823 - preg_replace: * qualifier on unicode replace garbles the string
--FILE--
<?php
var_dump(preg_replace('/[^\pL\pM]*/iu', '', 'áéíóú'));
// invalid UTF-8
var_dump(preg_replace('/[^\pL\pM]*/iu', '', "\xFCáéíóú"));
var_dump(preg_replace('/[^\pL\pM]*/iu', '', "áéíóú\xFC"));
?>
--EXPECTF--
string(10) "áéíóú"

Notice: preg_replace(): PCRE error 4 in %s%ebug53823.php on line %d
NULL

Notice: preg_replace(): PCRE error 4 in %s%ebug53823.php on line %d
NULL
