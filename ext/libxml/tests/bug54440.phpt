--TEST--
Bug #54440: libxml extension ignores default context
--SKIPIF--
<?php if (!extension_loaded('simplexml')) die('skip simplexml required for this test'); ?>
--FILE--
<?php

class TestWrapper {

function stream_open($path, $mode, $options, &$opened_path)
{
    if ($this->context)
        print_r(stream_context_get_options($this->context));
    return false;
}

function url_stat($path, $flags)
{
return array();
}

}

stream_wrapper_register("test", "TestWrapper")
    or die("Failed to register protocol");

$ctx1 = stream_context_create(array('test'=>array('test'=>'test 1')));
$ctx2 = stream_context_create(array('test'=>array('test'=>'test 2')));

stream_context_set_default(stream_context_get_options($ctx1));
@simplexml_load_file('test://sdfsdf');

libxml_set_streams_context($ctx2);
@simplexml_load_file('test://sdfsdf');
?>
--EXPECT--
Array
(
    [test] => Array
        (
            [test] => test 1
        )

)
Array
(
    [test] => Array
        (
            [test] => test 2
        )

)
