--TEST--
std handles can be deliberately closed 003
--SKIPIF--
<?php
if (php_sapi_name() != "cli") {
	die("skip CLI only");
}
if (PHP_OS_FAMILY == 'Windows') {
	die("skip not for Windows");
}
?>
--FILE--
<?php

$stdoutStream = fopen('php://stdout', 'r');

$stdoutFile = tempnam(sys_get_temp_dir(), 'gh8827');
$stderrFile = tempnam(sys_get_temp_dir(), 'gh8827');
register_shutdown_function(function () use ($stdoutFile, $stderrFile) {
    unlink($stdoutFile);
    unlink($stderrFile);
});

fclose(STDOUT);
fclose(STDERR);

$stdoutFileStream = fopen($stdoutFile, 'a');
$stderrFileStream = fopen($stderrFile, 'a');

print "Goes to stdoutFile\n";
file_put_contents('php://fd/1', "Also goes to stdoutFile\n");

file_put_contents('php://fd/2', "Goes to stderrFile\n");

ob_start(function ($buffer) use ($stdoutStream) {
    fwrite($stdoutStream, $buffer);
}, 1);

print "stdoutFile:\n";
readfile($stdoutFile);

print "stderrFile:\n";
readfile($stderrFile);

?>
--EXPECT--
stdoutFile:
Goes to stdoutFile
Also goes to stdoutFile
stderrFile:
Goes to stderrFile
