--TEST--
Deprecated doubleval() alias
--INI--
precision = 14
--FILE--
<?php

$values = [
	null,
	false,
	true,
	10,
	"string",
	[],
	STDOUT,
	new stdClass(),
];

foreach ($values as $val) {
	var_dump(doubleval($val) === floatval($val));
}
?>
--EXPECTF--
Deprecated: Function doubleval() is deprecated since 8.6, use floatval() instead in %s on line %d
bool(true)

Deprecated: Function doubleval() is deprecated since 8.6, use floatval() instead in %s on line %d
bool(true)

Deprecated: Function doubleval() is deprecated since 8.6, use floatval() instead in %s on line %d
bool(true)

Deprecated: Function doubleval() is deprecated since 8.6, use floatval() instead in %s on line %d
bool(true)

Deprecated: Function doubleval() is deprecated since 8.6, use floatval() instead in %s on line %d
bool(true)

Deprecated: Function doubleval() is deprecated since 8.6, use floatval() instead in %s on line %d
bool(true)

Deprecated: Function doubleval() is deprecated since 8.6, use floatval() instead in %s on line %d
bool(true)

Deprecated: Function doubleval() is deprecated since 8.6, use floatval() instead in %s on line %d

Warning: Object of class stdClass could not be converted to float in %s on line %d

Warning: Object of class stdClass could not be converted to float in %s on line %d
bool(true)
