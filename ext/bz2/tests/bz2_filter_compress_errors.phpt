--TEST--
bzip2.compress filter param errors
--EXTENSIONS--
bz2
--FILE--
<?php
$fp = fopen('php://stdout', 'w');

$param = 'not an array';
var_dump(stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE, $param));

$param = ['blocks' => 'not an int'];
var_dump(stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE, $param));

$param = ['blocks' => '15'];
var_dump(stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE, $param));

$param = ['blocks' => '0'];
var_dump(stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE, $param));

$param = ['work' => 'not an int'];
var_dump(stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE, $param));

$param = ['work' => '251'];
var_dump(stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE, $param));

$param = ['work' => '-1'];
var_dump(stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE, $param));

fclose($fp);

?>
--EXPECTF--
Warning: stream_filter_append(): Filter parameters for bzip2.compress filter must be of type array|object, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)

Warning: stream_filter_append(): Number of blocks parameter must be of type int, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)

Warning: stream_filter_append(): Number of blocks to allocate must be between 1 and 9, 15 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)

Warning: stream_filter_append(): Number of blocks to allocate must be between 1 and 9, 0 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)

Warning: stream_filter_append(): Work factor parameter must be of type int, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)

Warning: stream_filter_append(): Work factor must be between 0 and 250, 251 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)

Warning: stream_filter_append(): Work factor must be between 0 and 250, -1 given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)
