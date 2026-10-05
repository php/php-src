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

$fp = fopen('php://memory', 'r+');
// Create two separate Terminal objects wrapping the same memory stream
$term1 = Terminal::fromStreams($fp);
$term2 = Terminal::fromStreams($fp);

// Write partial UTF-8 prefix \xE2
fwrite($fp, "\xE2");
rewind($fp);

// Terminal 1 reads \xE2 and times out; pending UTF-8 buffer belongs to $term1
$k1 = $term1->readKey(Duration::fromSeconds(0));
var_dump($k1 === null);

// Terminal 2 now attempts to read the stream without $term1's decoder state
fwrite($fp, "\x82\xAC");
fseek($fp, 1);

// $term2 sees continuation bytes without the initial prefix
$k2 = $term2->readKey(Duration::fromSeconds(0));
// $term2 cannot assemble the € symbol because it does not share $term1's pending buffer
var_dump($k2 !== "€");

// $term1, given its continuation, assembles the full sequence
fseek($fp, 1);
$k1_res = $term1->readKey(Duration::fromSeconds(0));
var_dump($k1_res === "€");

fclose($fp);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
