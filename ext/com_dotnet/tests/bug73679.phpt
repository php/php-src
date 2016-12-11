--TEST--
Bug #73679 DOTNET read access violation using invalid codepage 
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present"; ?>
--FILE--
<?php

$stack = new DOTNET("mscorlib", "System.Collections.Stack", -2200000000);
$stack->Push(".Net");
$stack->Push("Hello ");
echo $stack->Pop() . $stack->Pop();

?>
--EXPECTF--
Fatal error: Uncaught com_exception: Could not create .Net object - invalid codepage! in %sbug73679.php:%d
Stack trace:
#0 %sbug73679.php(%d): dotnet->dotnet('mscorlib', 'System.Collecti...', -2200000000)
#1 {main}
  thrown in %sbug73679.php on line %d
