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
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(["a\x00b" => 1] === []);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert("tab\there" === "");
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(str_contains("plain", "zzz"));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert(!str_contains($string, "\000"))
AssertionError: assert(["a\000b" => 1] === [])
AssertionError: assert("tab\there" === '')
AssertionError: assert(str_contains('plain', 'zzz'))
