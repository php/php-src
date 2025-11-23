--TEST--
GH-20370 (User filters should respect typed properties)
--FILE--
<?php

class pass_filter
{
    public $filtername;
    public $params;
    public int $stream = 1;

    function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}

stream_filter_register("pass", "pass_filter");
$fp = fopen("php://memory", "w");
stream_filter_append($fp, "pass");

try {
    fwrite($fp, "data");
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

try {
    fclose($fp);
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

unset($fp); // prevent cleanup at shutdown

?>
--EXPECTF--
Warning: fwrite(): Unprocessed filter buckets remaining on input brigade in %s on line %d
TypeError: Cannot assign resource to property pass_filter::$stream of type int
TypeError: Cannot assign resource to property pass_filter::$stream of type int
