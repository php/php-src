--TEST--
Bug #77428: mb_ereg_replace() doesn't replace a substitution variable 
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg_replace')) die('skip mb_ereg_replace() not available');
?>
--FILE--
<?php

// This behavior is broken, but kept for BC reasons
var_dump(mb_ereg_replace('(%)', '\\\1', 'a%c'));
// For clarity, the above line is equivalent to:
var_dump(mb_ereg_replace('(%)', '\\\\1', 'a%c'));

?>
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, Oniguruma functions support ends PHP 9.0 in %s on line %d
string(4) "a\%%c"

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, Oniguruma functions support ends PHP 9.0 in %s on line %d
string(4) "a\%%c"
