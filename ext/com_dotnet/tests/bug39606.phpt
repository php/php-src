--TEST--
COM: Loading typelib corrupts memory
--SKIPIF--
<?php
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present"; ?>
--FILE--
<?php
error_reporting(E_ALL);

$arEnv = array_change_key_case($_SERVER, CASE_UPPER);

$root = dirname($arEnv['COMSPEC']);
$typelib = $root.'\activeds.tlb';

var_dump(com_load_typelib($typelib));
var_dump(com_load_typelib($typelib));
?>
===DONE===
--EXPECT--
bool(true)
bool(true)
===DONE===
