--TEST--
Check if multiple filters are closed correctly and never called again after close
--FILE--
<?php

class FirstFilter extends php_user_filter {
    public function filter($in, $out, &$consumed, $closing): int {
        static $closed = 0;

        while ($bucket = stream_bucket_make_writeable($in)) {
            stream_bucket_append($out, stream_bucket_new($this->stream, $bucket->data));
        }

        if ($closing) {
            $closed++;
        }

        if ($closed > 0) {
            var_dump($closed++);
        }
        return PSFS_PASS_ON;
    }
}

class SecondFilter extends php_user_filter {
    public function filter($in, $out, &$consumed, $closing): int {
        static $closed = 0;

        while ($bucket = stream_bucket_make_writeable($in)) {
            stream_bucket_append($out, stream_bucket_new($this->stream, $bucket->data));
        }

        if ($closing) {
            $closed++;
        }

        if ($closed > 0) {
            var_dump($closed++);
        }
        return PSFS_PASS_ON;
    }
}

$r = fopen("php://stdout", "w+");
stream_filter_register("first", "FirstFilter");
stream_filter_register("second", "SecondFilter");
$first = stream_filter_prepend($r, "first", STREAM_FILTER_WRITE, []);
$second = stream_filter_prepend($r, "second", STREAM_FILTER_WRITE, []);
fwrite($r, "test\n");
stream_filter_remove($second);
stream_filter_remove($first);
?>
--EXPECT--
test
int(1)
int(1)
