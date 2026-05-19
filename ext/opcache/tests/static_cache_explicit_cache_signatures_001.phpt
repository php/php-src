--TEST--
OPcache explicit cache signatures expose public API types
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
		'false' => 7,
	];
	$names = array_map(
		static fn (ReflectionNamedType $named): string => $named->getName(),
		$type->getTypes(),
	);
	usort($names, static fn (string $left, string $right): int => ($order[$left] ?? 99) <=> ($order[$right] ?? 99));

	return implode('|', $names);
}

foreach ([
	'OPcache\\volatile_store' => ['value', 'ttl', 'throw_on_error'],
	'OPcache\\volatile_store_array' => ['ttl', 'throw_on_error'],
	'OPcache\\volatile_fetch' => ['default', 'throw_on_error'],
	'OPcache\\volatile_fetch_array' => ['default', 'throw_on_error'],
	'OPcache\\volatile_exists' => ['throw_on_error'],
	'OPcache\\volatile_lock' => ['lease', 'throw_on_error'],
	'OPcache\\volatile_unlock' => ['throw_on_error'],
	'OPcache\\volatile_delete' => ['throw_on_error'],
	'OPcache\\volatile_delete_array' => ['throw_on_error'],
	'OPcache\\volatile_clear' => ['throw_on_error'],
	'OPcache\\volatile_cache_info' => [],
	'OPcache\\pinned_store' => ['value', 'throw_on_error'],
	'OPcache\\pinned_store_array' => ['throw_on_error'],
	'OPcache\\pinned_fetch' => ['default', 'throw_on_error'],
	'OPcache\\pinned_fetch_array' => ['default', 'throw_on_error'],
	'OPcache\\pinned_exists' => ['throw_on_error'],
	'OPcache\\pinned_lock' => ['lease', 'throw_on_error'],
	'OPcache\\pinned_unlock' => ['throw_on_error'],
	'OPcache\\pinned_delete' => ['throw_on_error'],
	'OPcache\\pinned_delete_array' => ['throw_on_error'],
	'OPcache\\pinned_clear' => ['throw_on_error'],
	'OPcache\\pinned_atomic_increment' => ['step', 'throw_on_error'],
	'OPcache\\pinned_atomic_decrement' => ['step', 'throw_on_error'],
	'OPcache\\pinned_cache_info' => [],
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
OPcache\volatile_store $value=null|bool|int|float|string|array|object $ttl=int $throw_on_error=bool params=2/4 return=bool
OPcache\volatile_store_array $ttl=int $throw_on_error=bool params=1/3 return=bool
OPcache\volatile_fetch $default=null|bool|int|float|string|array|object $throw_on_error=bool params=1/3 return=null|bool|int|float|string|array|object
OPcache\volatile_fetch_array $default=?array $throw_on_error=bool params=1/3 return=array|false
OPcache\volatile_exists $throw_on_error=bool params=1/2 return=bool
OPcache\volatile_lock $lease=int $throw_on_error=bool params=1/3 return=bool
OPcache\volatile_unlock $throw_on_error=bool params=1/2 return=bool
OPcache\volatile_delete $throw_on_error=bool params=1/2 return=bool
OPcache\volatile_delete_array $throw_on_error=bool params=1/2 return=bool
OPcache\volatile_clear $throw_on_error=bool params=0/1 return=bool
OPcache\volatile_cache_info params=0/0 return=OPcache\StaticCacheInfo
OPcache\pinned_store $value=null|bool|int|float|string|array|object $throw_on_error=bool params=2/3 return=bool
OPcache\pinned_store_array $throw_on_error=bool params=1/2 return=bool
OPcache\pinned_fetch $default=null|bool|int|float|string|array|object $throw_on_error=bool params=1/3 return=null|bool|int|float|string|array|object
OPcache\pinned_fetch_array $default=?array $throw_on_error=bool params=1/3 return=array|false
OPcache\pinned_exists $throw_on_error=bool params=1/2 return=bool
OPcache\pinned_lock $lease=int $throw_on_error=bool params=1/3 return=bool
OPcache\pinned_unlock $throw_on_error=bool params=1/2 return=bool
OPcache\pinned_delete $throw_on_error=bool params=1/2 return=bool
OPcache\pinned_delete_array $throw_on_error=bool params=1/2 return=bool
OPcache\pinned_clear $throw_on_error=bool params=0/1 return=bool
OPcache\pinned_atomic_increment $step=int $throw_on_error=bool params=1/3 return=int|false
OPcache\pinned_atomic_decrement $step=int $throw_on_error=bool params=1/3 return=int|false
OPcache\pinned_cache_info params=0/0 return=OPcache\StaticCacheInfo
