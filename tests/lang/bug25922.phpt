--TEST--
Bug #25922 (SEGV in error_handler when context is destroyed)
--INI--
error_reporting=2047
--FILE--
<?php
function my_error_handler($error, $errmsg='', $errfile='', $errline=0, $errcontext='')
{
	$errcontext = '';
}
                                                                                        
set_error_handler('my_error_handler');

function test()
{
	echo "Undefined index here: '{$data['HTTP_HEADER']}'\n";
}
test();
?>
--EXPECTF--
Fatal error: User error handler must not modify error context in %sbug25922.php on line 11
