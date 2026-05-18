--TEST--
Chunked encoding (multiple buckets)
--SKIPIF--
<?php
$filters = stream_get_filters();
if(! in_array( "dechunk", $filters )) die( "skip Chunked filter not available." );
?>
--INI--
allow_url_fopen=1
--FILE--
<?php

// Filter that splits each bucket in the input into two, with $chunkSize bytes in the first bucket.
final class Splitter extends php_user_filter {
    public static int $chunkSize;
    function filter($in, $out, &$consumed, $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $head = substr($bucket->data, 0, self::$chunkSize);
            $head_bucket = stream_bucket_new($this->stream, $head);
            stream_bucket_append($out, $head_bucket);

            $tail = substr($bucket->data, self::$chunkSize);
            $tail_bucket = stream_bucket_new($this->stream, $tail);
            stream_bucket_append($out, $tail_bucket);

            $consumed += strlen($bucket->data);
            return PSFS_PASS_ON;
        }
        return PSFS_FEED_ME;
    }
}
stream_filter_register("Splitter", "Splitter");

$testdata = "2;key=value\r\nte\r\n2\r\nst\r\n0\r\nTrailer: section\r\n\r\n";

// Split test data on every possible position
for ($i = 1; $i < strlen($testdata); $i++) {
    Splitter::$chunkSize = $i;

    $fp = fopen("data:text/plain,$testdata", "r");
    stream_filter_append($fp, "Splitter", STREAM_FILTER_READ);
    stream_filter_append($fp, "dechunk", STREAM_FILTER_READ);

    var_dump(stream_get_contents($fp));
    fclose($fp);
}
?>
--EXPECT--
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
string(4) "test"
