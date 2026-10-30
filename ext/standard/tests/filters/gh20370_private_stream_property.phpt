--TEST--
GH-20370 (User filters should handle private stream property correctly)
--FILE--
<?php

class pass_filter
{
    public $filtername;
    public $params;
    private $stream;

    function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }

    function onClose()
    {
        var_dump($this->stream); // should be null
    }
}

stream_filter_register("pass", "pass_filter");
$fp = fopen("php://memory", "w");
stream_filter_append($fp, "pass", STREAM_FILTER_WRITE);

fwrite($fp, "data");
rewind($fp);
echo fread($fp, 1024) . "\n";

?>
--EXPECT--
data
NULL
