--TEST--
GH-23259 (Stream-error finalization uses streams after close)
--FILE--
<?php
class OversizedReadStream
{
    public $context;
    private bool $read = false;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_read(int $count): string
    {
        $this->read = true;
        return str_repeat('A', $count + 1);
    }

    public function stream_eof(): bool
    {
        return $this->read;
    }

    public function stream_stat(): array
    {
        return [];
    }
}

stream_wrapper_register('oversized-read', OversizedReadStream::class);
var_dump(get_meta_tags('oversized-read://input'));
?>
--EXPECTF--
Warning: get_meta_tags(): OversizedReadStream::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost in %s on line %d
array(0) {
}
