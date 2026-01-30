--TEST--
sscanf(): format string which is unterminated
--FILE--
<?php

function combinatorial_concat(string $base, array $patterns): array {
	$r = [$base];
	foreach ($patterns as $pattern) {
		$r = array_merge(
			...array_map(
				fn (string $str) => [$str, $str . $pattern],
				$r,
			)
		);
	}
	return $r;
}

$modifiers = [
	'*',
	'1$', // XPG
	'2', // Width
	'L', // Size
];

$formats = combinatorial_concat('%', $modifiers);

$str = "Hello World";

foreach ($formats as $format) {
	echo "Using format string '$format':\n";
	try {
	    sscanf($str, $format);
	} catch (Throwable $e) {
	    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}
?>
--EXPECT--
Using format string '%':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%L':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%2':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%2L':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%1$':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%1$L':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%1$2':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%1$2L':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%*':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%*L':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%*2':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%*2L':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%*1$':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%*1$L':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%*1$2':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
Using format string '%*1$2L':
ValueError: sscanf(): Argument #2 ($format) unterminated format specifier
