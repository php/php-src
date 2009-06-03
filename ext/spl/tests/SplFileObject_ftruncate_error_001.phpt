--TEST--
SplFileObject::ftruncate function - truncating with stream that does not support truncation 
--FILE--
<?php

//create a basic stream class
class VariableStream {
    var $position;
    var $varname;

    function stream_open($path, $mode, $options, &$opened_path)
    {
        return true;
    }
}
stream_wrapper_register("SPLtest", "VariableStream");
$ftruncate_test = "";
//end creating stream

//open an SplFileObject using the above test stream
$obj = New SplFileObject("SPLtest://ftruncate_test"); 
try {
	$obj->ftruncate(1);
} catch (LogicException $e) {
	echo($e->getMessage());
}
?>
--EXPECTF--
Can't truncate file %s
