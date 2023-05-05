--TEST--
GH-10072 (PHP crashes when execute_ex is overridden and a trampoline is used from internal code)
--EXTENSIONS--
zend_test
--INI--
zend_test.replace_zend_execute_ex=1
opcache.jit=disable
--FILE--
<?php
class DummyStreamWrapper
{
    /** @var resource|null */
    public $context;

    /** @var resource|null */
    public $handle;


    public function stream_cast(int $castAs)
    {
        return $this->handle;
    }


    public function stream_close(): void
    {
    }

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


    public function stream_write(string $data)
    {
    }


    public function unlink(string $path): bool
    {
        return false;
    }
}

class TrampolineTest {
    /** @var resource|null */
    public $context;

    /** @var object|null */
    private $wrapper;

    public function __call(string $name, array $arguments) {
        if (!$this->wrapper) {
            $this->wrapper = new DummyStreamWrapper();
        }
        echo 'Trampoline for ', $name, PHP_EOL;
        return $this->wrapper->$name(...$arguments);
	}

}

stream_wrapper_register('custom', TrampolineTest::class);


$fp = fopen("custom://myvar", "r+");
?>
--EXPECT--
Trampoline for stream_open
Trampoline for stream_close
