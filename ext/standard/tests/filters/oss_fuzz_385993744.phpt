--TEST--
OSS-Fuzz #385993744
--FILE--
<?php

class SampleFilter extends php_user_filter
{
    private $data = '';

    public function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in))
        {
            $this->data .= $bucket->data;
        }

        $bucket = stream_bucket_new($this->stream, $this->data);
        stream_bucket_append($out, $bucket);

        return PSFS_FEED_ME;
    }
}
stream_filter_register('sample.filter', SampleFilter::class);
var_dump(file_get_contents('php://filter/read=sample.filter/resource='. __FILE__));

?>
--EXPECT--
string(0) ""
