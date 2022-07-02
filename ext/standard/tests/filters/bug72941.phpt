--TEST--
Bug #72941 (Modifying bucket->data by-ref has no effect any longer)
--FILE--
<?php
class rotate_filter_nw extends php_user_filter
{
    function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $this->rotate($bucket->data);
            $consumed += $bucket->datalen;
            stream_bucket_prepend($out, $bucket);
        }

        return PSFS_PASS_ON;
    }

    function rotate(&$data)
    {
        $n = strlen($data);
        for ($i = 0; $i < $n - 1; ++$i) {
            $data[$i] = $data[$i + 1];
        }
    }
}

stream_filter_register("rotator_notWorking", rotate_filter_nw::class);
$stream = fopen('php://memory', 'w+');
fwrite($stream, 'hello, world');
rewind($stream);
stream_filter_append($stream, "rotator_notWorking");
var_dump(stream_get_contents($stream));
?>
--EXPECT--
string(12) "ello, worldd"
