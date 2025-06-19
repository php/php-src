--TEST--
GH-18823 (setlocale's 2nd and 3rd argument ignores strict_types) - strict mode
--FILE--
<?php
declare(strict_types=1);
try {
    setlocale(LC_ALL, 0, "0");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    setlocale(LC_ALL, "0", 0);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
setlocale(): Argument #2 ($locales) must be of type array|string, int given
setlocale(): Argument #3 must be of type array|string, int given
