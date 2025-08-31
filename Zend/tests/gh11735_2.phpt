--TEST--
GH-11735: Use-after-free when unregistering user stream wrapper from user stream wrapper
--FILE--
<?php
class FooWrapper {
    public $context;
    public function stream_open($path, $mode, $options, &$opened_path) {
        stream_wrapper_unregister('foo');
        return false;
    }
}
stream_wrapper_register('foo', 'FooWrapper');
var_dump(fopen('foo://bar', 'r'));
?>
--EXPECTF--
Warning: fopen(foo://bar): Failed to open stream: "FooWrapper::stream_open" call failed in %s on line %d
bool(false)
