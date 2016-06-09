--TEST--
Uncatched exceptions are properly stored.
--FILE--
<?php

register_shutdown_function(function () {
	$error = error_get_last();
	assert($error['type'] === 1);
	assert(preg_match(
		'/^Uncaught Exception: DateTime::__construct\(\): Failed to parse time string \(1\/1\/11111\) at position 8 \(1\): Unexpected character in .*\nStack trace:\n#0 .*: DateTime->__construct\(\'1\/1\/11111\'\)\n#1 {main}\n  thrown$/D',
		$error['message']
	) === 1);
	assert(!empty($error['file']));
	assert(is_int($error['line']));
});

new DateTime('1/1/11111');

?>
--EXPECTF--
Fatal error: Uncaught Exception: DateTime::__construct(): Failed to parse time string (1/1/11111) at position 8 (1): Unexpected character in %s
Stack trace:
#0 %s: DateTime->__construct('1/1/11111')
#1 {main}
  thrown in %s
