--TEST--
Bug #77578 (Crash when php unload)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
// To actually be able to verify the crash during shutdown on Windows, we have
// to execute a PHP subprocess, and check its exit status.
$php = PHP_BINARY;
$extension_dir = ini_get("extension_dir");
$script = <<<SCRIPT
if (!extension_loaded('com_dotnet')) dl('com_dotnet');
ini_set('com.autoregister_typelib', '1');
new COM('WbemScripting.SWbemLocator');
SCRIPT;
$command = "$php -d extension_dir=$extension_dir -r \"$script\"";
exec($command, $output, $status);
var_dump($output, $status);
?>
--EXPECT--
array(0) {
}
int(0)
