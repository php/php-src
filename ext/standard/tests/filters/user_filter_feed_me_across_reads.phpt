--TEST--
Input kept back by a read filter with PSFS_FEED_ME survives across reads and is released on close
--SKIPIF--
<?php
if (!function_exists('stream_socket_pair') || !defined('STREAM_PF_UNIX') || PHP_OS_FAMILY === 'Windows') die('skip no unix socket pair');
?>
--FILE--
<?php
class defer extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        $data = '';
        while ($b = stream_bucket_make_writeable($in)) { $data .= $b->data; }
        if (!$closing && strlen($data) < 8) {
            if ($data !== '') {
                stream_bucket_prepend($in, stream_bucket_new($this->stream, $data));
            }
            return PSFS_FEED_ME;
        }
        stream_bucket_append($out, stream_bucket_new($this->stream, "[" . $data . "]"));
        return PSFS_PASS_ON;
    }
}
stream_filter_register("defer", "defer");
[$r, $w] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_blocking($r, false);
stream_filter_append($r, "defer", STREAM_FILTER_READ);
fwrite($w, "abc");
var_dump(fread($r, 100));
fwrite($w, "defgh");
var_dump(fread($r, 100));
fwrite($w, "xy");
var_dump(fread($r, 100));
fclose($r);
fclose($w);
?>
--EXPECT--
string(0) ""
string(10) "[abcdefgh]"
string(0) ""
