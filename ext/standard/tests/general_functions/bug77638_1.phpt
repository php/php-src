--TEST--
Bug #77638 (var_export'ing certain class instances segfaults)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
var_export(new COM("Scripting.Dictionary"));
?>
--EXPECT--
\com::__set_state(array(
))
