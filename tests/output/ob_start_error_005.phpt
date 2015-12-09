--TEST--
ob_start(): ensure buffers can't be added from within callback.
--FILE--
<?php

/* 
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/ 

function f($str) {
	ob_start();
	echo "hello";
	ob_end_flush();
	return $str;
}


var_dump(ob_start('f'));
echo "done";
?>
--EXPECTF--
Fatal error: ob_start(): Cannot use output buffering in output buffering display handlers in %s on line 9
