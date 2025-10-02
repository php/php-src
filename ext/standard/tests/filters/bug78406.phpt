--TEST--
Bug #78406: Broken file includes with user-defined stream filters
--FILE--
<?php

echo "bug\n"; // Should be transformed by filter on second include

if (!class_exists(SampleFilter::class)) {
    class SampleFilter extends php_user_filter
    {
        private $data = '';

        public function filter($in, $out, &$consumed, $closing): int
        {
            while ($bucket = stream_bucket_make_writeable($in))
            {
                $this->data .= $bucket->data;
            }

            if ($closing || feof($this->stream))
            {
                $consumed = strlen($this->data);

                $this->data = str_replace('bug', 'feature', $this->data);

                $bucket = stream_bucket_new($this->stream, $this->data);
                stream_bucket_append($out, $bucket);

                return PSFS_PASS_ON;
            }

            return PSFS_FEED_ME;
        }
    }
    stream_filter_register('sample.filter', SampleFilter::class);
    $uri = 'php://filter/read=sample.filter/resource='. __FILE__;

    include $uri; // We expect one more "feature" output at line 3
}

?>
--EXPECT--
bug
feature
