--TEST--
dechunk filter: seeking to position 0 resets the filter's internal state
--SKIPIF--
<?php
if (!in_array('dechunk', stream_get_filters(), true)) die('skip dechunk filter not available');
?>
--FILE--
<?php

$body = "8\r\nSymfony \r\n5\r\nis aw\r\n6\r\nesome!\r\n0\r\n\r\n";

// no seek, post-terminator writes are discarded
$h = fopen('php://temp', 'w+');
stream_filter_append($h, 'dechunk', STREAM_FILTER_WRITE);
fwrite($h, $body);
$before = ftell($h);
fwrite($h, '-');
var_dump(ftell($h) === $before);

// rewind() resets the filter: subsequent writes are processed as a
// new chunked stream, so invalid input like "-" leaks through
$h = fopen('php://temp', 'w+');
stream_filter_append($h, 'dechunk', STREAM_FILTER_WRITE);
fwrite($h, $body);
rewind($h);
$before = ftell($h);
fwrite($h, '-');
var_dump(ftell($h) > $before);

// stream_get_contents($h, -1, 0) performs an implicit seek to 0
$h = fopen('php://temp', 'w+');
stream_filter_append($h, 'dechunk', STREAM_FILTER_WRITE);
fwrite($h, $body);
stream_get_contents($h, -1, 0);
$before = ftell($h);
fwrite($h, '-');
var_dump(ftell($h) > $before);

// after reset, the filter correctly decodes a fresh chunked body
$h = fopen('php://temp', 'w+');
stream_filter_append($h, 'dechunk', STREAM_FILTER_WRITE);
fwrite($h, $body);
rewind($h);
ftruncate($h, 0);
fwrite($h, "1\r\nX\r\n0\r\n\r\n");
var_dump(stream_get_contents($h, -1, 0));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(1) "X"
