--TEST--
#[\Deprecated]: Deprecation converted to ErrorException does not break
--FILE--
<?php

function my_error_handler(int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
}

set_error_handler('my_error_handler');

#[\Deprecated]
interface DemoInterface {}

class DemoClass implements DemoInterface {}

?>
--EXPECTF--
Fatal error: Uncaught ErrorException: Interface DemoInterface implemented by DemoClass is deprecated in %s:%d
Stack trace:
#0 %s(%d): my_error_handler(16384, 'Interface DemoI...', '%s', 12)
#1 {main}
  thrown in %s on line %d
