--TEST--
sha1_file() returns false when a stream read fails
--FILE--
<?php
class TestStream
{
    public $context;
    private string $mode;
    private int $position = 0;

    public function stream_open(string $path): bool
    {
        $this->mode = substr($path, strlen('test://'));

        return true;
    }

    public function stream_read(int $count): string|false
    {
        $position = $this->position++;
        if ($this->mode === 'error') {
            return false;
        }
        if ($this->mode === 'empty' || $this->mode === 'stalled') {
            return '';
        }
        if ($this->mode === 'short') {
            return substr('prefix', $position * 2, 2);
        }
        if ($position === 0) {
            return 'prefix';
        }

        return $this->mode === 'prefix-error' ? false : '';
    }

    public function stream_eof(): bool
    {
        return $this->mode === 'empty'
            || ($this->mode === 'eof' && $this->position > 1)
            || ($this->mode === 'short' && $this->position >= 3);
    }
}

stream_wrapper_register('test', TestStream::class);

foreach (['error', 'prefix-error', 'stalled', 'prefix-stalled', 'empty', 'eof', 'short'] as $mode) {
    echo "$mode: ";
    var_dump(sha1_file("test://$mode"));
    echo "$mode (raw): ";
    $result = sha1_file("test://$mode", true);
    var_dump(is_string($result) ? bin2hex($result) : $result);
}

echo "directory: ";
var_dump(@sha1_file(__DIR__));
echo "directory (raw): ";
$result = @sha1_file(__DIR__, true);
var_dump(is_string($result) ? bin2hex($result) : $result);
?>
--EXPECT--
error: bool(false)
error (raw): bool(false)
prefix-error: bool(false)
prefix-error (raw): bool(false)
stalled: bool(false)
stalled (raw): bool(false)
prefix-stalled: bool(false)
prefix-stalled (raw): bool(false)
empty: string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
empty (raw): string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
eof: string(40) "b4ebfe34d0fa97f0dd2bb1234fad8f59805f4e8d"
eof (raw): string(40) "b4ebfe34d0fa97f0dd2bb1234fad8f59805f4e8d"
short: string(40) "b4ebfe34d0fa97f0dd2bb1234fad8f59805f4e8d"
short (raw): string(40) "b4ebfe34d0fa97f0dd2bb1234fad8f59805f4e8d"
directory: bool(false)
directory (raw): bool(false)
