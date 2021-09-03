--TEST--
Bug #77578 (Crash when php unload)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
// To actually be able to verify the crash during shutdown on Windows, we have
// to execute a PHP subprocess, and check its exit status.
$php = PHP_BINARY;
$ini = php_ini_loaded_file();
$iniopt = $ini ? "-c $ini" : '';
$command = "$php $iniopt -d extension=com_dotnet -d com.autoregister_typelib=1 -r \"new COM('WbemScripting.SWbemLocator');\"";
exec($command, $output, $status);
var_dump($output, $status);
?>
--EXPECT--
array(0) {
}
int(0)
