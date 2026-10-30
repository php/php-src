--TEST--
Disabled PCRE2 regex compilation features.
--FILE--
<?php

$subject = 'hello';
$patterns = [
	'/\u/', // Unknown unicode escape sequence
    '/\C/u', // \C in unicode
    '/a(?=b\K)/', // \K in look around
];

foreach ($patterns as $pattern) {
     var_dump(preg_match($pattern, $subject));
}
?>
--EXPECTF--
Warning: preg_match(): Compilation failed: PCRE2 does not support \F, \L, \l, \N{name}, \U, or \u at offset 2 in %s on line %d
bool(false)

Warning: preg_match(): Compilation failed: using \C is incompatible with the 'u' modifier at offset 2 in %s on line %d
bool(false)

Warning: preg_match(): Compilation failed: \K is not allowed in lookarounds at offset 8 in %s on line %d
bool(false)
