--TEST--
Closures cannot be instantiated directly
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--FILE--
<?php

try {
    // Closures should be instantiatable using new
    $x = new Closure();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage();
}

?>
--EXPECT--
Error: Instantiation of class Closure is not allowed
