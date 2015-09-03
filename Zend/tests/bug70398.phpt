--TEST--
Bug #70398 (SIGSEGV, Segmentation fault zend_ast_destroy_ex)
--FILE--
<?php

define("FILE_STREAM", fopen("php://temp", "r"));


$array = array(
	fopen("php://temp", "r"),
);

define("FILE_STREAMS", $array);
?>
OK
--EXPECT--
OK
