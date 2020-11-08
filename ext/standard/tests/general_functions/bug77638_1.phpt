--TEST--
Bug #77638 (var_export'ing certain class instances segfaults)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
?>
--FILE--
<?php
var_export(new COM("Scripting.Dictionary"));
?>
--EXPECT--
com::__set_state(array(
))
