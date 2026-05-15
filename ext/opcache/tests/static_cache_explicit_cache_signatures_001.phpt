--TEST--
OPcache explicit cache signatures expose storable values and array fallbacks
--EXTENSIONS--
opcache
--FILE--
<?php

function describe_type(?ReflectionType $type): string
{
	if ($type === null) {
		return 'none';
	}

	if ($type instanceof ReflectionNamedType) {
		$name = $type->getName();

		return $type->allowsNull() && $name !== 'null' && $name !== 'mixed' ? '?' . $name : $name;
	}

	$order = [
		'null' => 0,
		'bool' => 1,
		'int' => 2,
		'float' => 3,
		'string' => 4,
		'array' => 5,
		'object' => 6,
	];
	$names = array_map(
		static fn (ReflectionNamedType $named): string => $named->getName(),
		$type->getTypes(),
	);
	usort($names, static fn (string $left, string $right): int => ($order[$left] ?? 99) <=> ($order[$right] ?? 99));

	return implode('|', $names);
}

foreach ([
	'OPcache\\volatile_store' => ['value'],
	'OPcache\\volatile_fetch' => ['default'],
	'OPcache\\volatile_fetch_array' => ['default'],
	'OPcache\\persistent_store' => ['value'],
	'OPcache\\persistent_fetch' => ['default'],
	'OPcache\\persistent_fetch_array' => ['default'],
] as $function => $parameters) {
	$reflection = new ReflectionFunction($function);
	$parts = [$function];
	$parameterMap = [];
	foreach ($reflection->getParameters() as $parameter) {
		$parameterMap[$parameter->getName()] = $parameter;
	}
	foreach ($parameters as $parameterName) {
		$parameter = $parameterMap[$parameterName];
		$parts[] = '$' . $parameterName . '=' . describe_type($parameter->getType());
	}
	$parts[] = 'params=' . $reflection->getNumberOfRequiredParameters() . '/' . $reflection->getNumberOfParameters();
	$parts[] = 'return=' . describe_type($reflection->getReturnType());
	echo implode(' ', $parts), "\n";
}

?>
--EXPECT--
OPcache\volatile_store $value=null|bool|int|float|string|array|object params=2/3 return=bool
OPcache\volatile_fetch $default=null|bool|int|float|string|array|object params=1/2 return=null|bool|int|float|string|array|object
OPcache\volatile_fetch_array $default=?array params=1/2 return=?array
OPcache\persistent_store $value=null|bool|int|float|string|array|object params=2/2 return=void
OPcache\persistent_fetch $default=null|bool|int|float|string|array|object params=1/2 return=null|bool|int|float|string|array|object
OPcache\persistent_fetch_array $default=?array params=1/2 return=?array
