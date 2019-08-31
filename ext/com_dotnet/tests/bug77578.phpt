--TEST--
Bug #77578 (Crash when php unload)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
?>
--FILE--
<?php
// To actually be able to verify the crash during shutdown on Windows, we have
// to execute a PHP subprocess, and check its exit status.
$php = PHP_BINARY;
$ini = php_ini_loaded_file();
$iniopt = $ini ? "-c $ini" : '';
$command = "$php $iniopt -d com.autoregister_typelib=1 -r \"new COM('WbemScripting.SWbemLocator');\"";
exec($command, $output, $status);
var_dump($output, $status);
?>
===DONE===
--EXPECT--
array(0) {
}
int(0)
===DONE===
