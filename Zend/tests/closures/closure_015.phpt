--TEST--
Closure 015: converting to string/unicode
--FILE--
<?php

$x = function() { return 1; };
try {
    print (string) $x;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    print $x;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Object of class Closure could not be converted to string
Object of class Closure could not be converted to string
