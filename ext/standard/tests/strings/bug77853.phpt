--TEST--
Bug #77853: Inconsistent substr_compare behaviour with empty haystack
--FILE--
<?php

var_dump(substr_compare('', '', 0, 0));
var_dump(substr_compare('', '', 0));

var_dump(substr_compare('abc', '', 3, 0));
var_dump(substr_compare('abc', '', 3));

var_dump(substr_compare('abc', "\0", 3));

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
int(-1)
