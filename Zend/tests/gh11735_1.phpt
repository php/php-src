--TEST--
GH-11735: Use-after-free when unregistering user stream wrapper from user stream wrapper
--FILE--
<?php
class FooWrapper {
    public function stream_open($path, $mode, $options, &$opened_path) {
        stream_wrapper_unregister('foo');
        return true;
    }
}
stream_wrapper_register('foo', 'FooWrapper');
@fopen('foo://bar', 'r');
?>
--EXPECT--
