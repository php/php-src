--TEST--
Closure 015: converting to string/unicode
--FILE--
<?php

$x = function() { return 1; };
try {
    print (string) $x;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    print $x;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Object of class Closure could not be converted to string
Error: Object of class Closure could not be converted to string
