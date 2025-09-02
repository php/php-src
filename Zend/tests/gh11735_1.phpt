--TEST--
GH-11735: Use-after-free when unregistering user stream wrapper from user stream wrapper
--FILE--
<?php
class FooWrapper {
    public $context;
    public function stream_open($path, $mode, $options, &$opened_path) {
        stream_wrapper_unregister('foo');
        return true;
    }
}
stream_wrapper_register('foo', 'FooWrapper');
var_dump(fopen('foo://bar', 'r'));
?>
--EXPECTF--
resource(%d) of type (stream)
