--TEST--
Check for empty pattern
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
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
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Empty pattern
No regex given
