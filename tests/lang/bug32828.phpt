--TEST--
Bug #32828 (Throwing exception in output_callback function with ob_start and ob_end_clean leads to segfault)
--FILE--
<?php

function output_handler($buffer)
{
	throw new Exception;
}

ob_start('output_handler');

ob_end_clean();
?>
--EXPECTF--	
Exception: (empty message) in %s on line %d
Stack trace:
#0 [internal function]: output_handler('', %d)
#1 %s(%d): ob_end_clean()
#2 {main}
