--TEST--
Bug #35916 (Duplicate calls to stream_bucket_append() lead to a crash)
--FILE--
<?php
$file = __DIR__ . "/bug35916.txt";
@unlink($file);

class strtoupper_filter extends php_user_filter
{
        function filter($in, $out, &$consumed, $closing)
        {
        while($bucket=stream_bucket_make_writeable($in)) {
            $bucket->data = strtoupper($bucket->data);
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
            stream_bucket_append($out, $bucket);
                }
        return PSFS_PASS_ON;
        }
    function onCreate()
    {
        echo "fffffffffff\n";
    }
    function onClose()
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
