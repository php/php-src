--TEST--
preg_* with bogus vals
--FILE--
<?php

var_dump(preg_match_all('//', '', $dummy, 0xdead));

var_dump(preg_quote(''));

var_dump(preg_replace('/(.)/', '${1}${1', 'abc'));
var_dump(preg_replace('/.++\d*+[/', 'for ($', 'abc'));
var_dump(preg_replace('/(.)/e', 'for ($', 'abc'));

?>
--EXPECTF--
Warning: preg_match_all(): Invalid flags specified in %s002.php on line %d
NULL
string(0) ""
string(12) "a${1b${1c${1"

Warning: preg_replace(): Compilation failed: missing terminating ] for character class at offset 8 in %s002.php on line %d
NULL

Warning: preg_replace(): The /e modifier is no longer supported, use preg_replace_callback instead in %s on line %d
NULL
