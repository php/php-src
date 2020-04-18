--TEST--
Bug #77621 (Already defined constants are not properly reported)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
?>
--INI--
com.autoregister_verbose=1
--FILE--
<?php
define('ADSTYPE_INVALID', 0);
$root = dirname(array_change_key_case($_SERVER, CASE_UPPER)['COMSPEC']);
com_load_typelib("$root\activeds.tlb");
?>
--EXPECTF--
Warning: com_load_typelib(): Type library constant ADSTYPE_INVALID is already defined in %s on line %d
