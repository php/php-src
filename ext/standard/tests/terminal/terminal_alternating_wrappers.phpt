--TEST--
Io\Terminal\Terminal: separate Terminal wrappers own separate decoder state
--SKIPIF--
<?php
if (!class_exists(\Io\Terminal\Terminal::class)) die('skip Io\Terminal\Terminal not available');
if (!class_exists(\Time\Duration::class)) die('skip Time\Duration not available');
?>
--FILE--
<?php

use Io\Terminal\Terminal;
use Time\Duration;

[$reader, $writer] = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);

// Create two separate Terminal objects wrapping the same stream reader
$term1 = Terminal::fromStreams($reader);
$term2 = Terminal::fromStreams($reader);

// Write partial UTF-8 prefix \xE2 with writer remaining open
fwrite($writer, "\xE2");

// Terminal 1 reads \xE2 and times out; pending UTF-8 buffer belongs to $term1
$k1 = $term1->readKey(Duration::fromMilliseconds(20));
var_dump($k1 === null);

// Terminal 2 now attempts to read the shared stream without $term1's decoder state
fwrite($writer, "\x82\xAC");

// $term2 consumes \x82 and \xAC from the shared stream without initial prefix
$k2_1 = $term2->readKey(Duration::fromMilliseconds(20));
$k2_2 = $term2->readKey(Duration::fromMilliseconds(20));
// $term2 cannot assemble the € symbol because it does not share $term1's pending buffer
var_dump($k2_1 !== "€" && $k2_2 !== "€");

// Write continuation bytes \x82\xAC again so $term1 can complete the sequence
fwrite($writer, "\x82\xAC");
$k1_res = $term1->readKey(Duration::fromMilliseconds(20));
var_dump($k1_res === "€");

fclose($reader);
fclose($writer);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
