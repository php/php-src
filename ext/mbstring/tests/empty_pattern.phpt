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
--EXPECTF--
Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_ereg_search_init(): Argument #2 ($pattern) must not be empty

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_regs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
No pattern was provided
