--TEST--
Bug #64960 (Segfault in gc_zval_possible_root)
--FILE--
<?php
// this makes ob_end_clean raise an error
ob_end_flush();

class ExceptionHandler {
	public function __invoke (Exception $e)
	{
		// this triggers the custom error handler
		ob_end_clean();
	}
}

// this must be a class, closure does not trigger segfault
set_exception_handler(new ExceptionHandler());

// exception must be throwed from error handler.
set_error_handler(function()
{
	$e = new Exception;
	$e->_trace = debug_backtrace();
	
	throw $e;
});

// trigger error handler
$a['waa'];
?>
--EXPECTF--
Notice: ob_end_flush(): failed to delete and flush buffer. No buffer to delete or flush in %sbug64960.php on line 3

Fatal error: Uncaught exception 'Exception' in %sbug64960.php:19
Stack trace:
#0 [internal function]: {closure}(8, 'ob_end_clean():...', '%s', 9, Array)
#1 %sbug64960.php(9): ob_end_clean()
#2 [internal function]: ExceptionHandler->__invoke(Object(Exception))
#3 {main}
  thrown in %sbug64960.php on line 19
