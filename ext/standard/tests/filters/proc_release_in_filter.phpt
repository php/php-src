--TEST--
Releasing the process resource from a user filter's callback on one of its pipes
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
class Closer extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        global $proc;
        if ($proc !== null) {
            $proc = null;
            echo "released\n";
        }
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}

stream_filter_register('closer', 'Closer');
$proc = proc_open([PHP_BINARY, '-n', '-r', 'echo "abc";'], [1 => ['pipe', 'w']], $pipes);
stream_filter_append($pipes[1], 'closer', STREAM_FILTER_READ);
var_dump(fread($pipes[1], 10));
var_dump(fclose($pipes[1]));
?>
--EXPECT--
released
string(3) "abc"
bool(true)
