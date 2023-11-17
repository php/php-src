--TEST--
Error test for stream_context_set_options()
--FILE--
<?php
$bucket = stream_bucket_new(fopen('php://temp', 'w+'), '');

try {
    stream_context_set_options($bucket->bucket, []);
} catch (TypeError $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
stream_context_set_options(): Argument #1 ($context) must be a valid stream/context
