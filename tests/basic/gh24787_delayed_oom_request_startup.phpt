--TEST--
GH-24787 (Delayed OOM during request startup is handled as a fatal error)
--SKIPIF--
<?php
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die('skip php-cgi not available');
}
if (!extension_loaded('uri')) {
    die('skip uri extension not available');
}
?>
--FILE--
<?php
$script = tempnam(sys_get_temp_dir(), 'gh24787');
file_put_contents($script, "<?php echo \"request completed\\n\";");

$cookies = [];
for ($i = 0; $i < 2000; $i++) {
    $key = base_convert((string) $i, 10, 36);
    $value = $i < 1500 ? "xx" : "x";
    $cookies[] = 'c' . $key . '[0][0][0]=' . $value;
}

$process = proc_open(
    [getenv('TEST_PHP_CGI_EXECUTABLE'), '-n', '-d', 'memory_limit=2M', '-d', 'max_input_vars=10000', '-d', 'html_errors=0'],
    [STDIN, ['pipe', 'w'], ['pipe', 'w']],
    $pipes,
    null,
    ['REQUEST_METHOD' => 'GET', 'HTTP_COOKIE' => implode(';', $cookies), 'SCRIPT_FILENAME' => $script, 'REDIRECT_STATUS' => '1']
);
$output = stream_get_contents($pipes[1]) . stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
proc_close($process);
unlink($script);

echo str_contains($output, 'Allowed memory size') ? "OOM handled\n" : $output;
?>
--EXPECT--
OOM handled
