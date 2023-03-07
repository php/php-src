--TEST--
Test file_descriptor() function: error on uncastable user stream
--FILE--
<?php

// TODO Improve by mimicking tests from GH 10173 PR
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
        return 0;
    }

    public function stream_seek(int $offset, int $whence = SEEK_SET): bool
    {
        return true;
    }

    public function stream_set_option(int $option, int $arg1, ?int $arg2): bool
    {
        return false;
    }

    public function stream_stat()
    {
        return [];
    }

    public function stream_tell()
    {
        return [];
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
stream_wrapper_register('custom', DummyStreamWrapper::class);

$fp = fopen("custom://myvar", "r+");
try {
    var_dump(file_descriptor($fp));
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
fclose($fp);

echo "Done";
?>
--EXPECT--
file_descriptor(): Argument #1 ($stream) cannot represent as a file descriptor
Done
