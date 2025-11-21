--TEST--
GH-19685: bzip2.compress filter with invalid parameters should fail gracefully
--EXTENSIONS--
bz2
--FILE--
<?php
$stream = fopen('php://memory', 'w+');

// too low
$filter = stream_filter_append($stream, 'bzip2.compress', STREAM_FILTER_WRITE, array('blocks' => 0));
var_dump($filter);

// too high
$filter = stream_filter_append($stream, 'bzip2.compress', STREAM_FILTER_WRITE, array('blocks' => 10));
var_dump($filter);

// too low work
$filter = stream_filter_append($stream, 'bzip2.compress', STREAM_FILTER_WRITE, array('work' => -1));
var_dump($filter);

// too high work
$filter = stream_filter_append($stream, 'bzip2.compress', STREAM_FILTER_WRITE, array('work' => 251));
var_dump($filter);

fclose($stream);
?>
--EXPECTF--
Warning: stream_filter_append(): Invalid parameter given for number of blocks to allocate (0) in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)

Warning: stream_filter_append(): Invalid parameter given for number of blocks to allocate (10) in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)

Warning: stream_filter_append(): Invalid parameter given for work factor (-1) in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)

Warning: stream_filter_append(): Invalid parameter given for work factor (251) in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d
bool(false)
