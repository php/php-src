<?php

class DummyStreamWrapper
{
    /** @var resource|null */
    public $context;

    /** @var resource|null */
    public $handle;

    public function stream_cast(int $castAs)
    {
        return false;
    }

    public function stream_close(): void { }

    public function stream_open(string $path, string $mode, int $options = 0, ?string &$openedPath = null): bool
    {
        return true;
    }

    public function stream_read(int $count)
    {
        return false;
    }

    public function stream_seek(int $offset, int $whence = SEEK_SET): bool
    {
        var_dump('stream_seek!');
        return true;
    }

    public function stream_set_option(int $option, int $arg1, ?int $arg2): bool
    {
        return false;
    }

    public function stream_stat()
    {
        return false;
    }

    public function stream_tell()
    {
        return 0;
    }

    public function stream_truncate(int $newSize): bool
    {
        return true;
    }

    public function stream_write(string $data) { }


    public function unlink(string $path): bool
    {
        return false;
    }
}
