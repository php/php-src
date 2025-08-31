--TEST--
Check for empty pattern
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php

try {
    mb_ereg_search_init("","","");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

mb_split("","");

try {
    mb_ereg_search_regs();
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
mb_ereg_search_init(): Argument #2 ($pattern) must not be empty
No pattern was provided
