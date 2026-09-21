--TEST--
GH-22290: AST pretty printing does not correctly handle strings containing NUL
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

try {
	$string = "Foo\x00bar";
	assert(!str_contains($string, "\x00"));
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(["a\x00b" => 1] === []);
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert("tab\there" === "");
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(str_contains("plain", "zzz"));
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
assert(!str_contains($string, "\000"))
assert(["a\000b" => 1] === [])
assert("tab\there" === '')
assert(str_contains('plain', 'zzz'))
