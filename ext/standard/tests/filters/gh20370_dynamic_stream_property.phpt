--TEST--
GH-20370 (User filters should update dynamic stream property if it exists)
--FILE--
<?php

#[\AllowDynamicProperties]
class pass_filter
{
    public $filtername;
    public $params;

    function onCreate(): bool
    {
        $this->stream = null;
        return true;
    }

    function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        var_dump(property_exists($this, 'stream'));
        if (is_resource($this->stream)) {
            var_dump(get_resource_type($this->stream));
        }
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
--EXPECTF--
bool(true)
string(6) "stream"
bool(true)
string(6) "stream"
bool(true)
string(6) "stream"
bool(true)
string(6) "stream"
data
bool(true)
