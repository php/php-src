--TEST--
Bug #53823 - preg_replace: * qualifier on unicode replace garbles the string
--FILE--
<?php
var_dump(preg_replace('/[^\pL\pM]*/iu', '', 'áéíóú'));
?>
--EXPECT--
string(10) "áéíóú"
