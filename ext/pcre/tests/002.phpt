--TEST--
preg_* with bogus vals
--FILE--
<?php

var_dump(preg_match());
var_dump(preg_match_all());
var_dump(preg_match_all('//', '', $dummy, 0xdead));

var_dump(preg_quote());
var_dump(preg_quote(''));

var_dump(preg_replace('/(.)/', '${1}${1', 'abc'));
var_dump(preg_replace('/.++\d*+[/', 'for ($', 'abc'));
var_dump(preg_replace('/(.)/e', 'for ($', 'abc'));

?>
--EXPECTF--

Warning: preg_match() expects at least 2 parameters, 0 given in %s002.php on line 3
bool(false)

Warning: preg_match_all() expects at least 2 parameters, 0 given in %s002.php on line 4
bool(false)

Warning: preg_match_all(): Invalid flags specified in %s002.php on line 5
NULL

Warning: preg_quote() expects at least 1 parameter, 0 given in %s002.php on line 7
NULL
string(0) ""
string(12) "a${1b${1c${1"

Warning: preg_replace(): Compilation failed: missing terminating ] for character class at offset 8 in %s002.php on line 11
NULL

Warning: preg_replace(): The /e modifier is no longer supported, use preg_replace_callback instead in %s on line 12
NULL
