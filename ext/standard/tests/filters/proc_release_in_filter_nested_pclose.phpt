--TEST--
Releasing the process from a filter flushed by pclose() while a filter on its pipe is running
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
class PcloseOther extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        static $done = false;
        if (!$done) {
            $done = true;
            var_dump(pclose($GLOBALS['other']));
        }
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}

class ReleaseProc extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        if ($closing) {
            $GLOBALS['proc'] = null;
            echo "released\n";
        }
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}

stream_filter_register('pclose_other', 'PcloseOther');
stream_filter_register('release_proc', 'ReleaseProc');

// Child writes more than fits into the pipe buffer, so it blocks until the parent reads.
$proc = proc_open([PHP_BINARY, '-n', '-r', 'echo str_repeat("a", 1 << 20);'], [1 => ['pipe', 'w']], $pipes);
stream_filter_append($pipes[1], 'pclose_other', STREAM_FILTER_READ);

$other = popen(escapeshellarg(PHP_BINARY) . ' -n -r ' . escapeshellarg('stream_get_contents(STDIN);'), 'w');
stream_filter_append($other, 'release_proc', STREAM_FILTER_WRITE);

var_dump(strlen(fread($pipes[1], 10)));
var_dump(strlen(stream_get_contents($pipes[1])));
var_dump(fclose($pipes[1]));
?>
--EXPECT--
released
int(0)
int(10)
int(1048566)
bool(true)
