--TEST--
GH-22845 (Use-after-free when a user filter stashes its bucket brigade resource)
--FILE--
<?php
class stash_filter extends php_user_filter {
    public function filter($in, $out, &$consumed, $closing): int {
        $GLOBALS['stash_in'] = $in;
        $GLOBALS['stash_out'] = $out;
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += strlen($bucket->data);
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register('stash', 'stash_filter');

$fp = fopen('php://memory', 'r+');
fwrite($fp, "hello world");
rewind($fp);
stream_filter_append($fp, 'stash', STREAM_FILTER_READ);
var_dump(fread($fp, 100));

// The brigades lived on the filter caller's stack. Both stashed resources must be
// closed now that the callback has returned, not left pointing at freed stack.
foreach (['stash_in', 'stash_out'] as $name) {
    try {
        stream_bucket_make_writeable($GLOBALS[$name]);
        echo "$name: no error\n";
    } catch (\Throwable $e) {
        echo "$name: ", $e::class, "\n";
    }
}
echo "done\n";
?>
--EXPECT--
string(11) "hello world"
stash_in: TypeError
stash_out: TypeError
done
