--TEST--
Bug #55451 (substr_compare with NULL as default length)
--FILE--
<?php
var_dump(substr_compare("abcde", "ABCD", 0, NULL, false) != 0);
var_dump(substr_compare("abcde", "ABCD", 0, NULL, true) != 0);
var_dump(substr_compare("abcde", "ABCDE", 0, NULL, false) != 0);
var_dump(substr_compare("abcde", "ABCDE", 0, NULL, true) == 0);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
