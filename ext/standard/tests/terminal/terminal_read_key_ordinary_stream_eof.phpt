--TEST--
Io\Terminal\Terminal: readKey EOF behavior on ordinary streams
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

// 1. Immediate EOF on an empty ordinary stream across all timeout arguments:
// Default timeout
$fp = fopen('php://temp', 'r+');
$term = Terminal::fromStreams($fp);
try {
    $term->readKey();
    echo "FAIL: default timeout on empty stream did not throw\n";
} catch (TerminalException $e) {
    echo "PASS: default timeout immediate EOF: " . $e->getMessage() . "\n";
}
fclose($fp);

// Zero timeout
$fp = fopen('php://temp', 'r+');
$term = Terminal::fromStreams($fp);
try {
    $term->readKey(Duration::fromSeconds(0));
    echo "FAIL: zero timeout on empty stream did not throw\n";
} catch (TerminalException $e) {
    echo "PASS: zero timeout immediate EOF: " . $e->getMessage() . "\n";
}
fclose($fp);

// Positive timeout
$fp = fopen('php://temp', 'r+');
$term = Terminal::fromStreams($fp);
try {
    $term->readKey(Duration::fromMilliseconds(50));
    echo "FAIL: positive timeout on empty stream did not throw\n";
} catch (TerminalException $e) {
    echo "PASS: positive timeout immediate EOF: " . $e->getMessage() . "\n";
}
fclose($fp);

// 2. Reading an available character successfully, then encountering EOF:
// Zero timeout on subsequent read
$fp = fopen('php://temp', 'r+');
fwrite($fp, "A");
rewind($fp);
$term = Terminal::fromStreams($fp);
$key = $term->readKey(Duration::fromSeconds(0));
var_dump($key === "A");
try {
    $term->readKey(Duration::fromSeconds(0));
    echo "FAIL: subsequent zero timeout read did not throw\n";
} catch (TerminalException $e) {
    echo "PASS: subsequent zero timeout read EOF: " . $e->getMessage() . "\n";
}
fclose($fp);

// Positive timeout on subsequent read
$fp = fopen('php://temp', 'r+');
fwrite($fp, "B");
rewind($fp);
$term = Terminal::fromStreams($fp);
$key = $term->readKey(Duration::fromMilliseconds(50));
var_dump($key === "B");
try {
    $term->readKey(Duration::fromMilliseconds(50));
    echo "FAIL: subsequent positive timeout read did not throw\n";
} catch (TerminalException $e) {
    echo "PASS: subsequent positive timeout read EOF: " . $e->getMessage() . "\n";
}
fclose($fp);

// Default timeout on subsequent read
$fp = fopen('php://temp', 'r+');
fwrite($fp, "C");
rewind($fp);
$term = Terminal::fromStreams($fp);
$key = $term->readKey();
var_dump($key === "C");
try {
    $term->readKey();
    echo "FAIL: subsequent default timeout read did not throw\n";
} catch (TerminalException $e) {
    echo "PASS: subsequent default timeout read EOF: " . $e->getMessage() . "\n";
}
fclose($fp);

// 3. An open input with no data yet still returning null on timeout:
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
$socketTerm = Terminal::fromStreams($readStream);

// Zero timeout returns null immediately when open but no data
$kZero = $socketTerm->readKey(Duration::fromSeconds(0));
var_dump($kZero === null);

// Positive timeout returns null when open but no data
$kPos = $socketTerm->readKey(Duration::fromMilliseconds(30));
var_dump($kPos === null);

// Close writer to signal EOF on the open reader
fclose($writeStream);

// Now that writer is closed, reading encounters EOF and throws TerminalException
try {
    $socketTerm->readKey(Duration::fromSeconds(0));
    echo "FAIL: socket zero timeout EOF did not throw\n";
} catch (TerminalException $e) {
    echo "PASS: socket zero timeout EOF: " . $e->getMessage() . "\n";
}

try {
    $socketTerm->readKey(Duration::fromMilliseconds(30));
    echo "FAIL: socket positive timeout EOF did not throw\n";
} catch (TerminalException $e) {
    echo "PASS: socket positive timeout EOF: " . $e->getMessage() . "\n";
}

try {
    $socketTerm->readKey();
    echo "FAIL: socket default timeout EOF did not throw\n";
} catch (TerminalException $e) {
    echo "PASS: socket default timeout EOF: " . $e->getMessage() . "\n";
}

fclose($readStream);
?>
--EXPECT--
PASS: default timeout immediate EOF: End of file reached on terminal input stream
PASS: zero timeout immediate EOF: End of file reached on terminal input stream
PASS: positive timeout immediate EOF: End of file reached on terminal input stream
bool(true)
PASS: subsequent zero timeout read EOF: End of file reached on terminal input stream
bool(true)
PASS: subsequent positive timeout read EOF: End of file reached on terminal input stream
bool(true)
PASS: subsequent default timeout read EOF: End of file reached on terminal input stream
bool(true)
bool(true)
PASS: socket zero timeout EOF: End of file reached on terminal input stream
PASS: socket positive timeout EOF: End of file reached on terminal input stream
PASS: socket default timeout EOF: End of file reached on terminal input stream
