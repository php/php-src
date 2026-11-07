--TEST--
Io\Terminal\Terminal: ordinary-stream sequence timeout on partial Escape/CSI/UTF-8
--SKIPIF--
<?php
if (!class_exists(\Io\Terminal\Terminal::class)) die('skip Io\Terminal\Terminal not available');
if (!class_exists(\Time\Duration::class)) die('skip Time\Duration not available');
?>
--FILE--
<?php

use Io\Terminal\Key;
use Io\Terminal\Terminal;
use Time\Duration;

if (function_exists('pcntl_alarm')) {
    pcntl_alarm(5);
}

$pair = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
if ($pair === false) {
    die("Failed to create socket pair");
}

$readStream = $pair[0];
$writeStream = $pair[1];
$term = Terminal::fromStreams($readStream);

// Test 1: Standalone Escape on pipe with zero timeout returns Escape immediately
fwrite($writeStream, "\x1b");
$t0 = microtime(true);
$key0 = $term->readKey(Duration::fromSeconds(0), Duration::fromSeconds(0));
$elapsed0 = (microtime(true) - $t0) * 1000;
var_dump($key0 === Key::Escape);
var_dump($elapsed0 < 100);

// Test 2: Standalone Escape with sequence timeout returns Escape after sequence delay, not blocking forever
fwrite($writeStream, "\x1b");
$t1 = microtime(true);
$key1 = $term->readKey(Duration::fromMilliseconds(500), Duration::fromMilliseconds(60));
$elapsed1 = (microtime(true) - $t1) * 1000;
var_dump($key1 === Key::Escape);
var_dump($elapsed1 >= 40 && $elapsed1 < 400);

// Test 3: Partial CSI "\x1b[" on pipe returns after sequence timeout
fwrite($writeStream, "\x1b[");
$t2 = microtime(true);
$key2 = $term->readKey(Duration::fromMilliseconds(500), Duration::fromMilliseconds(60));
$elapsed2 = (microtime(true) - $t2) * 1000;
var_dump($key2 === "\x1b[");
var_dump($elapsed2 >= 40 && $elapsed2 < 400);

// Test 4: Incomplete UTF-8 prefix "\xE2" times out within deadline and retains prefix
fwrite($writeStream, "\xE2");
$t3 = microtime(true);
$key3 = $term->readKey(Duration::fromMilliseconds(60));
$elapsed3 = (microtime(true) - $t3) * 1000;
var_dump($key3 === null);
var_dump($elapsed3 >= 40 && $elapsed3 < 400);

// Supply completion bytes
fwrite($writeStream, "\x82\xAC");
$key4 = $term->readKey(Duration::fromMilliseconds(100));
var_dump($key4 === "€");

fclose($readStream);
fclose($writeStream);

// Test 5: Incomplete CSI sequence in readSecret times out within overall deadline
$pair5 = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
$read5 = $pair5[0];
$write5 = $pair5[1];
$term5 = Terminal::fromStreams($read5);

fwrite($write5, "\x1b[1;2;3");
$t5 = microtime(true);
$sec5 = $term5->readSecret(Duration::fromMilliseconds(150));
$elapsed5 = (microtime(true) - $t5) * 1000;
var_dump($sec5 === null);
var_dump($elapsed5 >= 100 && $elapsed5 < 600);

fclose($read5);
fclose($write5);

// Test 6: A producer continuously supplies non-final CSI bytes for roughly two seconds;
// readSecret with a 150ms timeout must return null well before producer finishes
$proc6 = proc_open(
    [PHP_BINARY, "-n", "-r", "fwrite(STDOUT, \"\\x1b[\"); \$t = microtime(true); while (microtime(true) - \$t < 2.0) { fwrite(STDOUT, \"1;2\"); usleep(5000); } fwrite(STDOUT, \"m\");"],
    [["pipe", "r"], ["pipe", "w"], ["pipe", "w"]],
    $pipes6
);

if (is_resource($proc6)) {
    $read6 = $pipes6[1];
    $term6 = Terminal::fromStreams($read6);
    $t6 = microtime(true);
    $sec6 = $term6->readSecret(Duration::fromMilliseconds(150));
    $elapsed6 = (microtime(true) - $t6) * 1000;
    var_dump($sec6 === null);
    var_dump($elapsed6 >= 100 && $elapsed6 < 800);
    fclose($pipes6[0]);
    fclose($pipes6[1]);
    fclose($pipes6[2]);
    proc_terminate($proc6);
    proc_close($proc6);
}

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
