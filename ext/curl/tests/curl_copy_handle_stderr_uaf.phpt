--TEST--
Cloning a curl handle keeps CURLOPT_STDERR tracked so a closed stream is detected on the clone too
--EXTENSIONS--
curl
--FILE--
<?php
$srcfile = __DIR__ . '/curl_copy_handle_stderr_uaf_src.txt';
file_put_contents($srcfile, "Test.");

$stderrfile = __DIR__ . '/curl_copy_handle_stderr_uaf_stderr.txt';
$stream = fopen($stderrfile, 'w+');

$ch1 = curl_init();
curl_setopt($ch1, CURLOPT_URL, "file://" . $srcfile);
curl_setopt($ch1, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch1, CURLOPT_VERBOSE, true);
curl_setopt($ch1, CURLOPT_STDERR, $stream);

$ch2 = clone $ch1;

fclose($stream);

var_dump(curl_exec($ch1));
var_dump(curl_exec($ch2));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/curl_copy_handle_stderr_uaf_src.txt');
@unlink(__DIR__ . '/curl_copy_handle_stderr_uaf_stderr.txt');
?>
--EXPECTF--
Warning: curl_exec(): CURLOPT_STDERR resource has gone away, resetting to stderr in %s on line %d
%A
string(5) "Test."

Warning: curl_exec(): CURLOPT_STDERR resource has gone away, resetting to stderr in %s on line %d
%A
string(5) "Test."
