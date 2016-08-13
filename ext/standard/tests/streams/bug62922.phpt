--TEST--
Bug #62922: Truncating entire string should result in string
--FILE--
<?php
var_dump(substr("", 0));
var_dump(substr("a", 1));
var_dump(substr("ab", 2));
?>
--EXPECT--
string(0) ""
string(0) ""
string(0) ""
