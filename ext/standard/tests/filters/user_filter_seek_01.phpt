--TEST--
php_user_filter with seek method - always seekable (stateless filter)
--FILE--
<?php

class RotateFilter extends php_user_filter
{
    private $rotation = 0;
    
    public function filter($in, $out, &$consumed, bool $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $rotated = '';
            for ($i = 0; $i < strlen($bucket->data); $i++) {
                $char = $bucket->data[$i];
                if (ctype_alpha($char)) {
                    $base = ctype_upper($char) ? ord('A') : ord('a');
                    $rotated .= chr($base + (ord($char) - $base + $this->rotation) % 26);
                } else {
                    $rotated .= $char;
                }
            }
            $bucket->data = $rotated;
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
    
    public function onCreate(): bool
    {
        if (isset($this->params['rotation'])) {
            $this->rotation = (int)$this->params['rotation'];
        }
        return true;
    }
    
    public function onClose(): void {}
    
    public function seek(int $offset, int $whence): bool
    {
        // Stateless filter - always seekable to any position
        return true;
    }
}

stream_filter_register('test.rotate', 'RotateFilter');

$file = __DIR__ . '/user_filter_seek_001.txt';
$text = 'Hello World';

file_put_contents($file, $text);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'test.rotate', STREAM_FILTER_READ, ['rotation' => 13]);

$partial = fread($fp, 5);
echo "First read: $partial\n";

$result = fseek($fp, 0, SEEK_SET);
echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

$full = fread($fp, strlen($text));
echo "Full content: $full\n";

$result = fseek($fp, 6, SEEK_SET);
echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

$from_middle = fread($fp, 5);
echo "Read from middle: $from_middle\n";

fclose($fp);
unlink($file);

?>
--EXPECT--
First read: Uryyb
Seek to start: SUCCESS
Full content: Uryyb Jbeyq
Seek to middle: SUCCESS
Read from middle: Jbeyq
