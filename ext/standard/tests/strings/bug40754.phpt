--TEST--
Bug #40754 (Overflow checks inside string functions)
--FILE--
<?php

$v = 2147483647;

var_dump(substr("abcde", 1, $v));
var_dump(substr_replace("abcde", "x", $v, $v));

var_dump(strspn("abcde", "abc", $v, $v));
var_dump(strcspn("abcde", "abc", $v, $v));

var_dump(substr_count("abcde", "abc", $v, $v));
var_dump(substr_compare("abcde", "abc", $v, $v));

var_dump(stripos("abcde", "abc", $v));
var_dump(substr_count("abcde", "abc", $v, 1));
var_dump(substr_count("abcde", "abc", 1, $v));
var_dump(strpos("abcde", "abc", $v));
var_dump(stripos("abcde", "abc", $v));
var_dump(strrpos("abcde", "abc", $v));
var_dump(strripos("abcde", "abc", $v));
var_dump(strncmp("abcde", "abc", $v));
var_dump(chunk_split("abcde", $v, "abc"));
var_dump(substr("abcde", $v, $v));

?>
--EXPECTF--	
%unicode|string%(4) "bcde"
%unicode|string%(6) "abcdex"
bool(false)
bool(false)

Warning: substr_count(): Offset value 2147483647 exceeds string length in %s on line %d
bool(false)

Warning: substr_compare(): The start position cannot exceed initial string length in %s on line %d
bool(false)

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: substr_count(): Offset value 2147483647 exceeds string length in %s on line %d
bool(false)

Warning: substr_count(): Length value 2147483647 exceeds string length in %s on line %d
bool(false)

Warning: strpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

Warning: strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
int(2)
string(8) "abcdeabc"
bool(false)
