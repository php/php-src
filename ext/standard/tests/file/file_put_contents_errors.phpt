--TEST--
file_put_contents() errors due to invalid parameters
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

$file = __DIR__."/file_put_contents.txt";

$context = stream_context_create();

// Invalid stream as data
try {
    var_dump(file_put_contents($file, $context));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(file_exists($file));

// Non stringable object as data
try {
    var_dump(file_put_contents($file, new stdClass()));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(file_exists($file));

// Invalid array as data
try {
    var_dump(file_put_contents($file, ['hello', new stdClass()]));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(file_exists($file));

// Invalid context resource for  stream context
try {
    var_dump(file_put_contents($file, "string", 0, STDERR));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(file_exists($file));
try {
    var_dump(file_put_contents($file, ["hello", ' world'], 0, STDERR));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(file_exists($file));

// Invalid "path" for lock 1
stream_wrapper_register('foo', DummyStreamWrapper::class);
try {
    var_dump(file_put_contents('foo://file.txt', "string", LOCK_EX));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(file_exists($file));
try {
    var_dump(file_put_contents('foo://file.txt', ["hello", ' world'], LOCK_EX));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(file_exists($file));

?>
--EXPECTF--
TypeError: file_put_contents(): supplied resource is not a valid stream resource
bool(false)
Error: Object of class stdClass could not be converted to string
bool(false)
Error: Object of class stdClass could not be converted to string
bool(false)
TypeError: file_put_contents(): supplied resource is not a valid Stream-Context resource
bool(false)
TypeError: file_put_contents(): supplied resource is not a valid Stream-Context resource
bool(false)

Warning: file_put_contents(): Exclusive locks may only be set for regular files in %s on line %d
bool(false)
bool(false)

Warning: file_put_contents(): Exclusive locks may only be set for regular files in %s on line %d
bool(false)
bool(false)
