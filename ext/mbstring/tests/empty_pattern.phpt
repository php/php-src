--TEST--
Check for empty pattern
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php

mb_ereg_search_init("","","");
mb_split("","");
mb_ereg_search_regs();

?>
--EXPECTF--
Warning: mb_ereg_search_init(): Empty pattern in %s on line %d

Warning: mb_ereg_search_regs(): No regex given in %s on line %d
