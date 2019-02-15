--TEST--
SplFileObject::fflush function - basic test
--FILE--
<?php
/*
 * test a successful flush
*/
$obj = New SplFileObject(dirname(__FILE__).'/SplFileObject_testinput.csv');
var_dump($obj->fflush());

/*
 * test a unsuccessful flush
*/
//create a basic stream class
class VariableStream {
    var $position;
    var $varname;

    function stream_open($path, $mode, $options, &$opened_path)
    {
        return true;
    }

    function url_stat() {
    }
}
stream_wrapper_register("SPLtest", "VariableStream");
$ftruncate_test = "";
//end creating stream

//open an SplFileObject using the above test stream
$obj = New SplFileObject("SPLtest://ftruncate_test");
var_dump($obj->fflush());

?>
--EXPECT--
bool(true)
bool(false)
