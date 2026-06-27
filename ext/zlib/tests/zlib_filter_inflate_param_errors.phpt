--TEST--
zlib.inflate filter param errors
--EXTENSIONS--
zlib
--FILE--
<?php
$fp = fopen('php://stdout', 'w');

$param = 'not an array';
var_dump(stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_WRITE, $param));

$param = ['window' => 'not an int'];
var_dump(stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_WRITE, $param));

$param = ['window' => -16];
var_dump(stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_WRITE, $param));

$param = ['window' => 48];
var_dump(stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_WRITE, $param));

fclose($fp);

?>
--EXPECTF--
Warning: stream_filter_append(): Filter parameters for zlib.inflate filter must be of type array, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.inflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Window size must be of type int, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.inflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Window size must be between -15 and 47, -16 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.inflate" in %s on line %d
bool(false)

Warning: stream_filter_append(): Window size must be between -15 and 47, 48 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "zlib.inflate" in %s on line %d
bool(false)
