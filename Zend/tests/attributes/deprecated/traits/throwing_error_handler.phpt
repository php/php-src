--TEST--
#[\Deprecated]: Deprecation converted to ErrorException does not break
--FILE--
<?php

function my_error_handler(int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
}

set_error_handler('my_error_handler');

#[\Deprecated]
trait DemoTrait {}

class DemoClass {
	use DemoTrait;
}

?>
--EXPECTF--
Fatal error: Uncaught ErrorException: Trait DemoTrait used by DemoClass is deprecated in %s:%d
Stack trace:
#0 %s: my_error_handler(16384, 'Trait DemoTrait...', '%s', %d)
#1 {main}
  thrown in %s on line %d
