--TEST--
Bug #81015: Opcache optimization assumes wrong part of ternary operator in if-condition
--FILE--
<?php

function ternary(bool $enabled, ?string $value): void
{
	// the "true" part is not as trivial in the real case
	if ($enabled ? true : $value === null) {
		echo ($value ?? 'NULL') . "\n";
	} else {
		echo "INVALID\n";
	}
}

ternary(true, 'value');
ternary(true, null);
ternary(false, 'value');
ternary(false, null);

?>
--EXPECT--
value
NULL
INVALID
NULL
