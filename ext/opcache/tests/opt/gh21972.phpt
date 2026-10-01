--TEST--
GH-21972: Typed by-value return must not leak reference wrapper
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
declare(strict_types=1);

enum ValueType {
	case BOOL;
	case MIXED;
}

function applyDefinition(
	bool &$lazy = false,
	ValueType &$type = ValueType::MIXED,
	int &$flags = 0,
	?string &$default = null,
): void {
}

function getTypedValue(string $default, bool $lazy, ValueType $type): string {
	applyDefinition($lazy, $type, default: $default);
	return $default;
}

$value = getTypedValue('false', false, ValueType::BOOL);
var_dump(gettype($value));
var_dump(strtolower($value));
var_dump(strtolower(getTypedValue('FALSE', false, ValueType::BOOL)));
?>
--EXPECT--
string(6) "string"
string(5) "false"
string(5) "false"
