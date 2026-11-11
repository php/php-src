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

// The field count needed to reach the startup gap varies by build and platform.
$counts = range(1700, 1950, 4);
$rinitFailed = false;
$oomReported = false;
foreach ($counts as $count) {
    $post = [];
    for ($i = 0; $i < $count; $i++) {
        $post[] = 'p' . $i . '=' . str_repeat('y', 4000);
    }
    $body = implode('&', $post);

    $command = [
        getenv('TEST_PHP_CGI_EXECUTABLE'),
        '-n',
        '-d', 'memory_limit=8M',
        '-d', 'post_max_size=64M',
        '-d', 'max_input_vars=20000',
        '-d', 'html_errors=0',
    ];
    $environment = [
        'REQUEST_METHOD' => 'POST',
        'CONTENT_TYPE' => 'application/x-www-form-urlencoded',
        'CONTENT_LENGTH' => (string) strlen($body),
        'SCRIPT_FILENAME' => $script,
        'REDIRECT_STATUS' => '1',
    ];
    $process = proc_open($command, [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']], $pipes, null, $environment);
    fwrite($pipes[0], $body);
    fclose($pipes[0]);
    $output = stream_get_contents($pipes[1]) . stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($process);

    $rinitFailed = $rinitFailed || str_contains($output, 'request_startup()');
    $oomReported = $oomReported || str_contains($output, 'Allowed memory size');
}
unlink($script);

echo !$rinitFailed && $oomReported ? "OOM handled\n" : "RINIT failure or no OOM reported\n";
?>
--EXPECT--
OOM handled
