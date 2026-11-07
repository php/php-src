--TEST--
proc_close() of the process from a user filter's callback on one of its pipes
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
class Closer extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        global $proc;
        static $tried = false;
        if (!$tried) {
            $tried = true;
            var_dump(proc_close($proc));
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
var_dump(proc_close($proc));
?>
--EXPECTF--
Warning: proc_close(): cannot close the process, as one of its pipes must not be manually closed in %s on line %d
int(-1)
string(3) "abc"
int(0)
