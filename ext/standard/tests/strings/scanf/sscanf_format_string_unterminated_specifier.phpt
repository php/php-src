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
ValueError: Bad scan conversion character "
Using format string '%L':
ValueError: Bad scan conversion character "
Using format string '%2':
ValueError: Bad scan conversion character "
Using format string '%2L':
ValueError: Bad scan conversion character "
Using format string '%1$':
ValueError: Bad scan conversion character "
Using format string '%1$L':
ValueError: Bad scan conversion character "
Using format string '%1$2':
ValueError: Bad scan conversion character "
Using format string '%1$2L':
ValueError: Bad scan conversion character "
Using format string '%*':
ValueError: Bad scan conversion character "
Using format string '%*L':
ValueError: Bad scan conversion character "
Using format string '%*2':
ValueError: Bad scan conversion character "
Using format string '%*2L':
ValueError: Bad scan conversion character "
Using format string '%*1$':
ValueError: Bad scan conversion character "$"
Using format string '%*1$L':
ValueError: Bad scan conversion character "$"
Using format string '%*1$2':
ValueError: Bad scan conversion character "$"
Using format string '%*1$2L':
ValueError: Bad scan conversion character "$"
