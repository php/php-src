--TEST--
sscanf(): unterminated range [] specifier
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
	'^',
	']',
];

$formats = combinatorial_concat('%[', $modifiers);

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
Using format string '%[':
ValueError: sscanf(): Argument #2 ($format) unterminated [ format specifier
Using format string '%[]':
ValueError: sscanf(): Argument #2 ($format) unterminated [ format specifier
Using format string '%[^':
ValueError: sscanf(): Argument #2 ($format) unterminated [ format specifier
Using format string '%[^]':
ValueError: sscanf(): Argument #2 ($format) unterminated [ format specifier
