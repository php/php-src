--TEST--
php_user_filter with seek method - stateful filter
--FILE--
<?php

class CountingFilter extends php_user_filter
{
    private $count = 0;
    
    public function filter($in, $out, &$consumed, bool $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $modified = '';
            for ($i = 0; $i < strlen($bucket->data); $i++) {
                $modified .= $bucket->data[$i] . $this->count++;
            }
            $bucket->data = $modified;
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
    
    public function onCreate(): bool
    {
        return true;
    }
    
    public function onClose(): void {}
    
    public function seek(int $offset, int $whence): bool
    {
        if ($offset === 0 && $whence === SEEK_SET) {
            $this->count = 0;
            return true;
        }
        return false;
    }
}

stream_filter_register('test.counting', 'CountingFilter');

$file = __DIR__ . '/user_filter_seek_002.txt';
$text = 'ABC';

file_put_contents($file, $text);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'test.counting', STREAM_FILTER_READ);

$first = fread($fp, 10);
echo "First read: $first\n";

$result = fseek($fp, 0, SEEK_SET);
echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

$second = fread($fp, 10);
echo "Second read after seek: $second\n";
echo "Counts reset: " . ($first === $second ? "YES" : "NO") . "\n";

$result = fseek($fp, 1, SEEK_SET);
echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

fclose($fp);
unlink($file);

?>
--EXPECTF--
First read: A0B1C2
Seek to start: SUCCESS
Second read after seek: A0B1C2
Counts reset: YES

Warning: fseek(): Stream filter seeking for user-filter failed in %s on line %d
Seek to middle: FAILURE
