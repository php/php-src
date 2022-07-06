--TEST--
preg_* with bogus vals
--FILE--
<?php

try {
    preg_match_all('//', '', $dummy, 0xdead);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(preg_quote(''));

var_dump(preg_replace('/(.)/', '${1}${1', 'abc'));
var_dump(preg_replace('/.++\d*+[/', 'for ($', 'abc'));
var_dump(preg_replace('/(.)/e', 'for ($', 'abc'));

?>
--EXPECTF--
preg_match_all(): Argument #4 ($flags) must be a PREG_* constant
string(0) ""
string(12) "a${1b${1c${1"

Warning: preg_replace(): Compilation failed: missing terminating ] for character class at offset 8 in %s002.php on line %d
NULL

Warning: preg_replace(): The /e modifier is no longer supported, use preg_replace_callback instead in %s on line %d
NULL
