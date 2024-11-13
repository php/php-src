--TEST--
Overwrite libxml_set_streams_context
--EXTENSIONS--
simplexml
--FILE--
<?php

// Based on bug54440.phpt

class TestWrapper {
    function stream_open($path, $mode, $options, &$opened_path)
    {
        print_r(stream_context_get_options($this->context));
        return false;
    }

    function url_stat($path, $flags)
    {
        return array();
    }
}

stream_wrapper_register("test", "TestWrapper");

$ctx1 = stream_context_create(array('test'=>array('test'=>'test 1')));
$ctx2 = stream_context_create(array('test'=>array('test'=>'test 2')));

libxml_set_streams_context($ctx2);
@simplexml_load_file('test://sdfsdf');

libxml_set_streams_context($ctx1);
@simplexml_load_file('test://sdfsdf');
?>
--EXPECT--
Array
(
    [test] => Array
        (
            [test] => test 2
        )

)
Array
(
    [test] => Array
        (
            [test] => test 1
        )

)
