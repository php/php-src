--TEST--
Io\Terminal\Terminal: ordinary-stream timeout on empty pipe with open writer
--SKIPIF--
<?php
if (!class_exists(\Io\Terminal\Terminal::class)) die('skip Io\Terminal\Terminal not available');
if (!class_exists(\Time\Duration::class)) die('skip Time\Duration not available');
?>
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalException;
use Time\Duration;

// Set a safety timeout so test fails rather than hangs runner
if (function_exists('pcntl_alarm')) {
    pcntl_alarm(5);
}

// Create a bidirectional socket pair or pipe where writer remains open
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

// Test 1: Empty pipe with zero timeout returns null immediately (not blocking)
$t0 = microtime(true);
$key0 = $term->readKey(Duration::fromSeconds(0));
$elapsed0 = (microtime(true) - $t0) * 1000;
var_dump($key0 === null);
var_dump($elapsed0 < 200);

// Test 2: Empty pipe with finite timeout returns null after bounded wait
$t1 = microtime(true);
$key1 = $term->readKey(Duration::fromMilliseconds(50));
$elapsed1 = (microtime(true) - $t1) * 1000;
var_dump($key1 === null);
var_dump($elapsed1 >= 30 && $elapsed1 < 300);

// Test 3: Queued byte is immediately readable
fwrite($writeStream, "Z");
$t2 = microtime(true);
$key2 = $term->readKey(Duration::fromMilliseconds(200));
$elapsed2 = (microtime(true) - $t2) * 1000;
var_dump($key2 === "Z");
var_dump($elapsed2 < 100);

// Test 4: readSecret on empty pipe with finite timeout returns null after bounded wait
$t3 = microtime(true);
$sec3 = $term->readSecret(Duration::fromMilliseconds(50));
$elapsed3 = (microtime(true) - $t3) * 1000;
var_dump($sec3 === null);
var_dump($elapsed3 >= 30 && $elapsed3 < 300);

fclose($readStream);
fclose($writeStream);

// Part 2: Anonymous pipe via proc_open
$proc = proc_open(
    [PHP_BINARY, '-r', 'usleep(2000000);'],
    [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
    $procPipes
);
if (!is_resource($proc)) {
    die("Failed to proc_open child");
}

$pipeTerm = Terminal::fromStreams($procPipes[1]);

// Test 5: Empty anonymous pipe with zero timeout returns null immediately (not blocking)
$t4 = microtime(true);
$key4 = $pipeTerm->readKey(Duration::fromSeconds(0));
$elapsed4 = (microtime(true) - $t4) * 1000;
var_dump($key4 === null);
var_dump($elapsed4 < 200);

// Test 6: Empty anonymous pipe with finite timeout returns null after bounded wait
$t5 = microtime(true);
$key5 = $pipeTerm->readKey(Duration::fromMilliseconds(50));
$elapsed5 = (microtime(true) - $t5) * 1000;
var_dump($key5 === null);
var_dump($elapsed5 >= 30 && $elapsed5 < 300);

// Test 7: readSecret on empty anonymous pipe with finite timeout returns null after bounded wait
$t6 = microtime(true);
$sec6 = $pipeTerm->readSecret(Duration::fromMilliseconds(50));
$elapsed6 = (microtime(true) - $t6) * 1000;
var_dump($sec6 === null);
var_dump($elapsed6 >= 30 && $elapsed6 < 300);

fclose($procPipes[0]);
fclose($procPipes[1]);
fclose($procPipes[2]);
proc_terminate($proc);
proc_close($proc);

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
bool(true)
