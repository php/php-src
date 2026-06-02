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
	'OPcache\\VolatileCache::set' => ['value', 'ttl'],
	'OPcache\\VolatileCache::setMultiple' => ['ttl'],
	'OPcache\\VolatileCache::get' => ['default'],
	'OPcache\\VolatileCache::getMultiple' => ['default'],
	'OPcache\\VolatileCache::has' => [],
	'OPcache\\VolatileCache::lock' => ['lease'],
	'OPcache\\VolatileCache::unlock' => [],
	'OPcache\\VolatileCache::delete' => [],
	'OPcache\\VolatileCache::deleteMultiple' => [],
	'OPcache\\VolatileCache::clear' => [],
	'OPcache\\VolatileCache::getCacheStoreType' => ['class_name'],
	'OPcache\\VolatileCache::info' => [],
	'OPcache\\PinnedCache::set' => ['value'],
	'OPcache\\PinnedCache::setMultiple' => [],
	'OPcache\\PinnedCache::get' => ['default'],
	'OPcache\\PinnedCache::getMultiple' => ['default'],
	'OPcache\\PinnedCache::has' => [],
	'OPcache\\PinnedCache::lock' => ['lease'],
	'OPcache\\PinnedCache::unlock' => [],
	'OPcache\\PinnedCache::delete' => [],
	'OPcache\\PinnedCache::deleteMultiple' => [],
	'OPcache\\PinnedCache::clear' => [],
	'OPcache\\PinnedCache::increment' => ['step'],
	'OPcache\\PinnedCache::decrement' => ['step'],
	'OPcache\\PinnedCache::getCacheStoreType' => ['class_name'],
	'OPcache\\PinnedCache::info' => [],
] as $method => $parameters) {
	$reflection = ReflectionMethod::createFromMethodName($method);
	$parts = [$method];
	$parameterMap = [];
	foreach ($reflection->getParameters() as $parameter) {
		$parameterMap[$parameter->getName()] = $parameter;
	}
	foreach ($parameters as $parameterName) {
		$parameter = $parameterMap[$parameterName];
		$parts[] = '$' . $parameterName . '=' . describe_type($parameter->getType());
	}
	$parts[] = 'static=' . ($reflection->isStatic() ? '1' : '0');
	$parts[] = 'params=' . $reflection->getNumberOfRequiredParameters() . '/' . $reflection->getNumberOfParameters();
	$parts[] = 'return=' . describe_type($reflection->getReturnType());
	echo implode(' ', $parts), "\n";
}

?>
--EXPECT--
OPcache\VolatileCache::set $value=null|bool|int|float|string|array|object $ttl=int static=1 params=2/3 return=bool
OPcache\VolatileCache::setMultiple $ttl=int static=1 params=1/2 return=bool
OPcache\VolatileCache::get $default=null|bool|int|float|string|array|object static=1 params=1/2 return=null|bool|int|float|string|array|object
OPcache\VolatileCache::getMultiple $default=?array static=1 params=1/2 return=array|false
OPcache\VolatileCache::has static=1 params=1/1 return=bool
OPcache\VolatileCache::lock $lease=int static=1 params=1/2 return=bool
OPcache\VolatileCache::unlock static=1 params=1/1 return=bool
OPcache\VolatileCache::delete static=1 params=1/1 return=bool
OPcache\VolatileCache::deleteMultiple static=1 params=1/1 return=bool
OPcache\VolatileCache::clear static=1 params=0/0 return=bool
OPcache\VolatileCache::getCacheStoreType $class_name=?string static=1 params=1/2 return=OPcache\CacheStoreType
OPcache\VolatileCache::info static=1 params=0/0 return=OPcache\StaticCacheInfo
OPcache\PinnedCache::set $value=null|bool|int|float|string|array|object static=1 params=2/2 return=bool
OPcache\PinnedCache::setMultiple static=1 params=1/1 return=bool
OPcache\PinnedCache::get $default=null|bool|int|float|string|array|object static=1 params=1/2 return=null|bool|int|float|string|array|object
OPcache\PinnedCache::getMultiple $default=?array static=1 params=1/2 return=array|false
OPcache\PinnedCache::has static=1 params=1/1 return=bool
OPcache\PinnedCache::lock $lease=int static=1 params=1/2 return=bool
OPcache\PinnedCache::unlock static=1 params=1/1 return=bool
OPcache\PinnedCache::delete static=1 params=1/1 return=bool
OPcache\PinnedCache::deleteMultiple static=1 params=1/1 return=bool
OPcache\PinnedCache::clear static=1 params=0/0 return=bool
OPcache\PinnedCache::increment $step=int static=1 params=1/2 return=int|false
OPcache\PinnedCache::decrement $step=int static=1 params=1/2 return=int|false
OPcache\PinnedCache::getCacheStoreType $class_name=?string static=1 params=1/2 return=OPcache\CacheStoreType
OPcache\PinnedCache::info static=1 params=0/0 return=OPcache\StaticCacheInfo
