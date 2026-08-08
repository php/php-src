--TEST--
ftp:// wrapper must reject control characters in the from address sent as the anonymous password
--SKIPIF--
<?php
if (array_search('ftp', stream_get_wrappers()) === false) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
?>
--FILE--
<?php

$log = tmpfile();

$socket = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
if (!$socket) {
    die("server failed\n");
}
$name = stream_socket_get_name($socket, false);
$port = (int) substr($name, strrpos($name, ':') + 1);

$pid = pcntl_fork();
if ($pid === 0) {
    pcntl_alarm(60);
    $s = stream_socket_accept($socket, 10);
    if (!$s) {
        exit(1);
    }
    fputs($s, "220 ready\r\n");
    stream_set_timeout($s, 1);
    $seen = '';
    while (($buf = fgets($s)) !== false) {
        $seen .= $buf;
        if (str_starts_with($buf, 'USER')) {
            fputs($s, "331 need pass\r\n");
        } else {
            fputs($s, "500 no\r\n");
        }
    }
    fwrite($log, $seen);
    exit(0);
}

ini_set('from', "evil\r\nSITE INJECT");
var_dump(@fopen("ftp://127.0.0.1:{$port}/x", "r"));
pcntl_waitpid($pid, $status);

rewind($log);
$seen = stream_get_contents($log);
var_dump(str_contains($seen, 'SITE INJECT'));
var_dump(str_contains($seen, 'PASS'));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
