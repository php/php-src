--TEST--
Catch exception in the nested multicatch
--FILE--
<?php

require_once __DIR__ . '/exceptions.inc';

try {
    try {
        echo 'TRY' . PHP_EOL;
        throw new Exception3;
    } catch (Exception1 | Exception3 $e) {
        echo get_class($e) . PHP_EOL;
    }
} catch(Exception2 | Exception3 $e) {
    echo 'Should never be executed';
} finally {
    echo 'FINALLY' . PHP_EOL;
}

?>
--EXPECT--
TRY
Exception3
FINALLY
