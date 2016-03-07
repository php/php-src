--TEST--
Catch first exception in the multicatch
--FILE--
<?php

require_once __DIR__ . '/exceptions.inc';

try {
	echo 'TRY' . PHP_EOL;
	throw new Exception1;
} catch(Exception1 | Exception2 | Exception3 $e) {
	echo get_class($e) . PHP_EOL;
} finally {
	echo 'FINALLY' . PHP_EOL;
}

?>
--EXPECT--
TRY
Exception1
FINALLY
