--TEST--
Catch second exception in the second multicatch
--FILE--
<?php
require_once __DIR__ . '/exceptions.inc';

try {
	echo 'TRY' . PHP_EOL;
	throw new Exception4;
} catch(Exception1 | Exception2 $e) {
	echo get_class($e) . PHP_EOL;
} catch(Exception3 | Exception4 $e) {
	echo get_class($e) . PHP_EOL;
} finally {
	echo 'FINALLY' . PHP_EOL;
}
?>
--EXPECT--
TRY
Exception4
FINALLY

