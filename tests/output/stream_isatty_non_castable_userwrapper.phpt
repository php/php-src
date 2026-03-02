--TEST--
Non castable user-space streams (stream_cast())
--FILE--
<?php
class test_wrapper {
    public $context;
    public $return_value;
    function stream_open($path, $mode, $openedpath) {
        return true;
    }
    function stream_eof() {
        return false;
    }
}

stream_wrapper_register('test', 'test_wrapper');
$fd = fopen("test://foo","r");
var_dump(stream_isatty($fd));

?>
--EXPECT--
bool(false)
