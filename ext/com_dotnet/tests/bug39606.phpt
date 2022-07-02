--TEST--
COM: Loading typelib corrupts memory
--EXTENSIONS--
com_dotnet
--FILE--
<?php
error_reporting(E_ALL);

$arEnv = array_change_key_case($_SERVER, CASE_UPPER);

$root = dirname($arEnv['COMSPEC']);
$typelib = $root.'\activeds.tlb';

var_dump(com_load_typelib($typelib));
var_dump(com_load_typelib($typelib));
?>
--EXPECT--
bool(true)
bool(true)
