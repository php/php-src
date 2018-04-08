--TEST--
Parsing test
--FILE--
<?php

require_once __DIR__ . '/exceptions.inc';

try {
	echo 'TRY' . PHP_EOL;
} catch(Exception1 | Exception2 $e) {
	echo 'Exception';
} finally {
	echo 'FINALLY' . PHP_EOL;
}

?>
--EXPECT--
TRY
FINALLY
