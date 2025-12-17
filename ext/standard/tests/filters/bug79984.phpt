--TEST--
Bug #79984 (Stream filter is not called with closing arg)
--FILE--
<?php

class F extends php_user_filter
{
    public function onCreate(): bool
    {
        echo 'filter onCreate' . PHP_EOL;
        return true;
    }

    public function onClose(): void
    {
        echo 'filter onClose' . PHP_EOL;
    }

    public function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $bucket->data = strtoupper($bucket->data);
            $consumed     += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        echo 'filtered ' . ($consumed ? $consumed : 0) . ' bytes';
        if ($closing) {
            echo ' and closing.';
        } else {
            echo '.';
        }
        if (feof($this->stream)) {
            echo ' Stream has reached end-of-file.';
        }
        echo PHP_EOL;
        return PSFS_PASS_ON;
    }
}

stream_filter_register('f', 'F');

$str = str_repeat('a', 8320);

$f2 = fopen('php://temp', 'r+b');
fwrite($f2, $str);
fseek($f2, 0, SEEK_SET);
stream_filter_append($f2, 'f', STREAM_FILTER_READ);
var_dump(strlen(stream_get_contents($f2)));
fclose($f2);

?>
--EXPECT--
filter onCreate
filtered 8192 bytes.
filtered 128 bytes and closing. Stream has reached end-of-file.
int(8320)
filter onClose
