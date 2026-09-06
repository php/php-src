--TEST--
Returning an object from stream_read() is invalid, but should not leak
--FILE--
<?php
class MyStream {
    public $context;
    function stream_open() {
        return true;
    }
    function stream_stat() {
        return false;
    }
    function stream_read() {
        return new stdClass;
    }
}
stream_wrapper_register('mystream', MyStream::class);
try {
    var_dump(file_get_contents('mystream://'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Object of class stdClass could not be converted to string
