--TEST--
Failure to send "100 Continue" is reported with ignore_user_abort=1
--SKIPIF--
<?php
include "skipif.inc";
if (!extension_loaded("sockets")) die("skip sockets extension required");
if (PHP_OS_FAMILY === "Windows") die("skip SO_LINGER reset behaviour differs on Windows");
?>
--FILE--
<?php
$log = tempnam(sys_get_temp_dir(), 'cli_server_log');
$log_fd = fopen($log, 'ab');
$server = proc_open(
    [getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY, '-n', '-d', 'ignore_user_abort=1', '-S', '127.0.0.1:0'],
    [0 => STDIN, 1 => $log_fd, 2 => $log_fd],
    $pipes,
    __DIR__
);

$port = null;
for ($i = 0; $i < 100 && $port === null; $i++) {
    usleep(50000);
    if (preg_match('@://127\.0\.0\.1:(\d+)\) started@', file_get_contents($log), $m)) {
        $port = $m[1];
    }
}

$fp = fsockopen('127.0.0.1', $port);
socket_set_option(socket_import_stream($fp), SOL_SOCKET, SO_LINGER, ['l_onoff' => 1, 'l_linger' => 0]);
fwrite($fp, "POST / HTTP/1.1\r\nExpect: 100-continue\r\nContent-Length: 4\r\n\r\n");
fclose($fp);

$output = '';
for ($i = 0; $i < 100 && !str_contains($output, 'Invalid request'); $i++) {
    usleep(50000);
    $output = file_get_contents($log);
}

var_dump(str_contains($output, 'Invalid request'), str_contains($output, 'Unexpected EOF'));

proc_terminate($server);
unlink($log);
?>
--EXPECT--
bool(true)
bool(false)
