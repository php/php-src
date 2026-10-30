--TEST--
GH-20370 (User filters should not create stream property if not declared)
--FILE--
<?php

class pass_filter
{
    public $filtername;
    public $params;

    function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }

        var_dump(property_exists($this, 'stream'));
        return PSFS_PASS_ON;
    }
}

stream_filter_register("pass", "pass_filter");
$fp = fopen("php://memory", "w");
stream_filter_append($fp, "pass");
fwrite($fp, "data");
rewind($fp);
echo fread($fp, 1024) . "\n";

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
data
bool(false)
