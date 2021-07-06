--TEST--
Offset errors for various strpos functions
--EXTENSIONS--
mbstring
--FILE--
<?php

try {
    var_dump(mb_strpos("f", "bar", 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strpos("f", "bar", -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strrpos("f", "bar", 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strrpos("f", "bar", -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_stripos("f", "bar", 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_stripos("f", "bar", -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strripos("f", "bar", 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strripos("f", "bar", -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
