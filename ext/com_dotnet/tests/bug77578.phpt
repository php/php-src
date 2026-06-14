--TEST--
Bug #77578 (Crash when php unload)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
// To actually be able to verify the crash during shutdown on Windows, we have
// to execute a PHP subprocess, and check its exit status.

// First we determine whether com_dotnet would be loaded in the subprocess
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$script = <<<SCRIPT
echo extension_loaded('com_dotnet') ? 'yes' : 'no';
SCRIPT;
exec("$php -r \"$script\"", $output);
var_dump(isset($output[0]));
$loaded = $output[0] === "yes";
$output = null;

// Then we run the subprocess with com_dotnet loaded
$script = <<<SCRIPT
ini_set('com.autoregister_typelib', '1');
new COM('WbemScripting.SWbemLocator');
SCRIPT;
if ($loaded) {
    $command = "$php -r \"$script\"";
} else {
    $extension_dir = escapeshellarg(ini_get("extension_dir"));
    $command = "$php -d extension_dir=$extension_dir -d extension=com_dotnet -r \"$script\"";
}
exec($command, $output, $status);
var_dump($output, $status);
?>
--EXPECT--
bool(true)
array(0) {
}
int(0)
