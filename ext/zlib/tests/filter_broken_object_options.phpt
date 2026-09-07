--TEST--
zlib filter assertion failure with non-dynamic properties in filter param object
--EXTENSIONS--
zlib
--FILE--
<?php

class Params {
    public int $memory = 6;
    public int $window = 15;
    public int $level = 6;
}

$fp = fopen('php://stdout', 'w');
stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE, new Params);
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_WRITE, new Params);
fwrite($fp, "Hello world, hopefully not broken\n");

?>
--EXPECTF--
Deprecated: stream_filter_append(): Passing an object for filter parameters for zlib.deflate is deprecated, call get_object_vars() first instead in %s on line %d

Deprecated: stream_filter_append(): Passing an object for filter parameters for zlib.inflate is deprecated, call get_object_vars() first instead in %s on line %d
Hello world, hopefully not broken
