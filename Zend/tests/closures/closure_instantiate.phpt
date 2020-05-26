--TEST--
Closures cannot be instantiated directly
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--FILE--
<?php

try {
    // Closures should be instantiatable using new
    $x = new Closure();
} catch (Exception $e) {
    // Instantiating a closure is an error, not an exception, so we shouldn't see this
    echo 'EXCEPTION: ', $e->getMessage();
} catch (Throwable $e) {
    // This is the message that we should see for a caught error
    echo 'ERROR: ', $e->getMessage();
}

?>
--EXPECT--
ERROR: Instantiation of class Closure is not allowed
