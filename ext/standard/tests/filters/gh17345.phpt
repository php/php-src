--TEST--
GH-17345 (Bug #35916 was not completely fixed)
--FILE--
<?php
$file = __DIR__ . "/gh17345.txt";
@unlink($file);

class strtoupper_filter extends php_user_filter
{
    function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket=stream_bucket_make_writeable($in)) {
            $bucket->data = strtoupper($bucket->data);
            $consumed += $bucket->datalen;
            stream_bucket_prepend($out, $bucket);
            // Interleave new bucket
            stream_bucket_prepend($out, clone $bucket);
            stream_bucket_prepend($out, $bucket);
        }
        return PSFS_PASS_ON;
    }

    function onCreate(): bool
    {
        echo "fffffffffff\n";
        return true;
    }

    function onClose(): void
    {
        echo "hello\n";
    }
}

stream_filter_register("strtoupper", "strtoupper_filter");
$fp=fopen($file, "w");
stream_filter_append($fp,  "strtoupper");
fread($fp, 1024);
fwrite($fp, "Thank you\n");
fclose($fp);
readfile($file);
unlink($file);
?>
--EXPECTF--
fffffffffff

Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
hello
THANK YOU
