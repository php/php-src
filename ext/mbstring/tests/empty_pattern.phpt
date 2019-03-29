--TEST--
Check for empty pattern
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_ereg_search_init')) die('skip mb_ereg_search_init() not available');
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
