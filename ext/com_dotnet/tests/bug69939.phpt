--TEST--
Bug #69939 (Casting object to bool returns false)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
var_dump((bool) new COM('WScript.Shell'));
?>
--EXPECT--
bool(true)
