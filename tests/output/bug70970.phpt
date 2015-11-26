--TEST--
Bug #70970 (Segfault when combining error handler with output buffering)
--FILE--
<?php
function exception_error_handler($severity, $message, $file, $line)
{
	throw new Exception($message, 0);
}

set_error_handler('exception_error_handler');

function obHandler($buffer, $phase = null)
{
	try {
		ob_start();
	} catch (Exception $e) {
		return (string) $e;
	}

	return $buffer;
}

ob_start('obHandler');

print 'test';
?>
--EXPECTF--
Fatal error: ob_start(): Cannot use output buffering in output buffering display handlers in %sbug70970.php on line %d
