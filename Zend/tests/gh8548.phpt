--TEST--
GH-8548: stream_wrapper_unregister() leaks memory
--FILE--
<?php

class Wrapper
{
    public $context;

    public function stream_open(string $path, string $mode, int $options): bool
    {
        return true;
    }
}

function test() {
    if (!stream_wrapper_register('foo', Wrapper::class)) {
        throw new \Exception('Could not register stream wrapper');
    }
    if (!stream_wrapper_unregister('foo')) {
        throw new \Exception('Could not unregister stream wrapper');
    }
}

// The first iterations will allocate space for things like the resource list
for ($i = 0; $i < 5; $i++) {
    test();
}

$before = memory_get_usage();
for ($i = 0; $i < 1000; $i++) {
    test();
}
$after = memory_get_usage();

var_dump($before === $after);

?>
--EXPECT--
bool(true)
