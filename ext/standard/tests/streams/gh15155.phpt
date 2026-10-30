--TEST--
GH-15155: Stream context is lost when custom stream wrapper is being filtered
--FILE--
<?php

class DummyWrapper
{
    public $context;

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool
    {
        $options = stream_context_get_options($this->context);
        var_dump($options['dummy']['foo']);
        return true;
    }

    public function stream_stat()
    {
    }

    public function stream_read()
    {
    }

    public function stream_eof()
    {
    }
}

$context = stream_context_create(['dummy' => ['foo' => 'bar']]);
stream_wrapper_register('dummy', DummyWrapper::class);
file_get_contents('dummy://foo', false, $context);
@file_get_contents('php://filter/resource=dummy://foo', false, $context);
?>
--EXPECT--
string(3) "bar"
string(3) "bar"
