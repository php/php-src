--TEST--
zlib.deflate filter param errors
--EXTENSIONS--
zlib
--FILE--
<?php
$fp = fopen('php://stdout', 'w');

$param = 'not an array or int';
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = -2;
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = 10;
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = ['memory' => 'not an int'];
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = ['memory' => 0];
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = ['memory' => 10];
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = ['window' => 'not an int'];
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = ['window' => -16];
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = ['window' => 32];
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = ['level' => 'not an int'];
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = ['level' => -2];
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

$param = ['level' => 10];
var_dump(stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, $param));

fclose($fp);

?>
--EXPECTF--
Warning: stream_filter_append(): Filter parameters for zlib.deflate filter must be of type array|int, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Compression level must be between -1 and 9, -2 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Compression level must be between -1 and 9, 10 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Window size must be of type int, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Memory level must be between 1 and 9, 0 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Memory level must be between 1 and 9, 10 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Window size must be of type int, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Window size must be between -15 and 31, -16 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Window size must be between -15 and 31, 32 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Compression level must be of type int, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Compression level must be between -1 and 9, -2 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Compression level must be between -1 and 9, 10 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.deflate" in %s on line %d
bool(false)
