--TEST--
Bug #77069 (stream filter loses final block of data)
--FILE--
<?php
class MyFilter extends php_user_filter {
    private $data = '';

    public function filter($in, $out, &$consumed, $closing) {
        $return = PSFS_FEED_ME;

        // While input data is available, continue to read it.
        while ($bucket_in = stream_bucket_make_writeable($in)) {
            $this->data .= $bucket_in->data;
            $consumed   += $bucket_in->datalen;

            // Process whole lines.
            while (preg_match('/(.*?)[\r\n]+(.*)/s', $this->data, $match) === 1) {
                list(, $data, $this->data) = $match;
                // Send this record output.
                $data       = strrev($data) . PHP_EOL;
                $bucket_out = stream_bucket_new($this->stream, $data);
                $return     = PSFS_PASS_ON;
                stream_bucket_append($out, $bucket_out);
            }
        }

        // Process the final line.
        if ($closing && $this->data !== '') {
            $data       = strrev($this->data) . PHP_EOL;
            $bucket_out = stream_bucket_new($this->stream, $data);
            $return     = PSFS_PASS_ON;
            stream_bucket_append($out, $bucket_out);
        }

        return $return;
    }
}

stream_filter_register('my-filter', 'MyFilter');

$input = "Line one\nLine two\nLine three";

$stream = fopen('data://text/plain,' . $input, 'r');
stream_filter_append($stream, 'my-filter');

$output = '';
while (!feof($stream)) {
    $output .= fread($stream, 16);
}
fclose($stream);

echo $output;
?>
--EXPECT--
eno eniL
owt eniL
eerht eniL
