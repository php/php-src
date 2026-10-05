--TEST--
Write filter returning PSFS_FEED_ME sees the input it kept back again with the next write
--FILE--
<?php
class defer extends php_user_filter {
    private int $kept = 0;
    public function filter($in, $out, &$consumed, bool $closing): int {
        $data = '';
        while ($b = stream_bucket_make_writeable($in)) { $data .= $b->data; }
        /* the bytes kept back from the previous call were already reported as consumed */
        $consumed += strlen($data) - $this->kept;
        if (!$closing && strlen($data) < 8) {
            $this->kept = strlen($data);
            stream_bucket_prepend($in, stream_bucket_new($this->stream, $data));
            return PSFS_FEED_ME;
        }
        $this->kept = 0;
        stream_bucket_append($out, stream_bucket_new($this->stream, "[" . $data . "]"));
        return PSFS_PASS_ON;
    }
}
stream_filter_register("defer", "defer");
$f = __DIR__ . "/user_filter_feed_me_write.bin";
$fp = fopen($f, 'w');
stream_filter_append($fp, "defer", STREAM_FILTER_WRITE);
var_dump(fwrite($fp, "abc"), fwrite($fp, "def"), fwrite($fp, "ghi"), fwrite($fp, "jk"));
fclose($fp);
var_dump(file_get_contents($f));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/user_filter_feed_me_write.bin");
?>
--EXPECT--
int(3)
int(3)
int(3)
int(2)
string(15) "[abcdefghi][jk]"
