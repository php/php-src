--TEST--
Bug #69939 (Casting object to bool returns false)
--SKIPIF--
<?php 
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present"; ?>
--FILE--
<?php
var_dump((bool) new COM('WScript.Shell'));
?>
--EXPECT--
bool(true)
