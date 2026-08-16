--TEST--
GH-16321 (UAF when stream filter throws during stream close)
--FILE--
<?php
class TestFilter extends php_user_filter {
    function filter($in, $out, &$consumed, $closing): int {
        stream_bucket_new($this->stream, "42");
        return PSFS_ERR_FATAL;
    }
}

stream_filter_register("test_filter", "TestFilter");

function test() {
    $stream = fopen('php://memory', 'wb+');
    fwrite($stream, "data");
    fseek($stream, 0, SEEK_SET);
    stream_filter_append($stream, "test_filter");
    stream_get_contents($stream);
}

test();
?>
--EXPECTF--
Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s on line %d

Fatal error: Uncaught TypeError: stream_bucket_new(): Argument #1 ($stream) must be an open stream resource in %s:%d
Stack trace:
#0 %s(%d): stream_bucket_new(Resource id #%d, '42')
#1 %s(%d): TestFilter->filter(Resource id #%d, Resource id #%d, 0, true)
#2 {main}
  thrown in %s on line %d
