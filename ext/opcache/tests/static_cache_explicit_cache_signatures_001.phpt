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
		'static' => 8,
	];
	$names = array_map(
		static fn (ReflectionNamedType $named): string => $named->getName(),
		$type->getTypes(),
	);
	usort($names, static fn (string $left, string $right): int => ($order[$left] ?? 99) <=> ($order[$right] ?? 99));

	return implode('|', $names);
}

foreach ([
	'OPcache\\StaticCacheInterface',
	'OPcache\\VolatileCache',
	'OPcache\\StableCache',
] as $class) {
	$reflection = new ReflectionClass($class);
	echo $class,
		' final=', $reflection->isFinal() ? '1' : '0',
		' instantiable=', $reflection->isInstantiable() ? '1' : '0',
		' interface=', $reflection->isInterface() ? '1' : '0',
		"\n";
}

foreach ([
	'OPcache\\StaticCacheInterface::getInstance' => ['pool_name'],
	'OPcache\\StaticCacheInterface::fetch' => ['default'],
	'OPcache\\StaticCacheInterface::fetchMultiple' => ['default'],
	'OPcache\\StaticCacheInterface::store' => ['value'],
	'OPcache\\StaticCacheInterface::storeMultiple' => [],
	'OPcache\\StaticCacheInterface::has' => [],
	'OPcache\\StaticCacheInterface::delete' => [],
	'OPcache\\StaticCacheInterface::deleteMultiple' => [],
	'OPcache\\StaticCacheInterface::clear' => [],
	'OPcache\\StaticCacheInterface::lock' => ['lease'],
	'OPcache\\StaticCacheInterface::unlock' => [],
	'OPcache\\StaticCacheInterface::getCacheStoreType' => [],
	'OPcache\\StaticCacheInterface::info' => [],
	'OPcache\\StaticCacheInterface::getCacheStoreTypeByProperty' => [],
	'OPcache\\StaticCacheInterface::getCacheStoreTypeByMethod' => [],
	'OPcache\\VolatileCache::getInstance' => ['pool_name'],
	'OPcache\\VolatileCache::fetch' => ['default'],
	'OPcache\\VolatileCache::fetchMultiple' => ['default'],
	'OPcache\\VolatileCache::store' => ['value'],
	'OPcache\\VolatileCache::storeMultiple' => [],
	'OPcache\\VolatileCache::has' => [],
	'OPcache\\VolatileCache::delete' => [],
	'OPcache\\VolatileCache::deleteMultiple' => [],
	'OPcache\\VolatileCache::clear' => [],
	'OPcache\\VolatileCache::lock' => ['lease'],
	'OPcache\\VolatileCache::unlock' => [],
	'OPcache\\VolatileCache::getCacheStoreType' => [],
	'OPcache\\VolatileCache::info' => [],
	'OPcache\\VolatileCache::getCacheStoreTypeByProperty' => [],
	'OPcache\\VolatileCache::getCacheStoreTypeByMethod' => [],
	'OPcache\\StableCache::getInstance' => ['pool_name'],
	'OPcache\\StableCache::fetch' => ['default'],
	'OPcache\\StableCache::fetchMultiple' => ['default'],
	'OPcache\\StableCache::store' => ['value'],
	'OPcache\\StableCache::storeMultiple' => [],
	'OPcache\\StableCache::storeWithTtl' => ['value', 'ttl'],
	'OPcache\\StableCache::storeMultipleWithTtl' => ['ttl'],
	'OPcache\\StableCache::has' => [],
	'OPcache\\StableCache::delete' => [],
	'OPcache\\StableCache::deleteMultiple' => [],
	'OPcache\\StableCache::clear' => [],
	'OPcache\\StableCache::lock' => ['lease'],
	'OPcache\\StableCache::unlock' => [],
	'OPcache\\StableCache::increment' => ['step'],
	'OPcache\\StableCache::decrement' => ['step'],
	'OPcache\\StableCache::getCacheStoreType' => [],
	'OPcache\\StableCache::info' => [],
	'OPcache\\StableCache::getCacheStoreTypeByProperty' => [],
	'OPcache\\StableCache::getCacheStoreTypeByMethod' => [],
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
OPcache\StaticCacheInterface final=0 instantiable=0 interface=1
OPcache\VolatileCache final=1 instantiable=0 interface=0
OPcache\StableCache final=1 instantiable=0 interface=0
OPcache\StaticCacheInterface::getInstance $pool_name=string static=1 params=1/1 return=static
OPcache\StaticCacheInterface::fetch $default=null|bool|int|float|string|array|object static=0 params=1/2 return=null|bool|int|float|string|array|object
OPcache\StaticCacheInterface::fetchMultiple $default=?array static=0 params=1/2 return=array|false
OPcache\StaticCacheInterface::store $value=null|bool|int|float|string|array|object static=0 params=2/2 return=bool
OPcache\StaticCacheInterface::storeMultiple static=0 params=1/1 return=bool
OPcache\StaticCacheInterface::has static=0 params=1/1 return=bool
OPcache\StaticCacheInterface::delete static=0 params=1/1 return=bool
OPcache\StaticCacheInterface::deleteMultiple static=0 params=1/1 return=bool
OPcache\StaticCacheInterface::clear static=0 params=0/0 return=bool
OPcache\StaticCacheInterface::lock $lease=int static=0 params=1/2 return=bool
OPcache\StaticCacheInterface::unlock static=0 params=1/1 return=bool
OPcache\StaticCacheInterface::getCacheStoreType static=0 params=1/1 return=OPcache\CacheStoreType
OPcache\StaticCacheInterface::info static=1 params=0/0 return=OPcache\StaticCacheInfo
OPcache\StaticCacheInterface::getCacheStoreTypeByProperty static=1 params=2/2 return=OPcache\CacheStoreType
OPcache\StaticCacheInterface::getCacheStoreTypeByMethod static=1 params=3/3 return=OPcache\CacheStoreType
OPcache\VolatileCache::getInstance $pool_name=string static=1 params=1/1 return=static
OPcache\VolatileCache::fetch $default=null|bool|int|float|string|array|object static=0 params=1/2 return=null|bool|int|float|string|array|object
OPcache\VolatileCache::fetchMultiple $default=?array static=0 params=1/2 return=array|false
OPcache\VolatileCache::store $value=null|bool|int|float|string|array|object static=0 params=2/2 return=bool
OPcache\VolatileCache::storeMultiple static=0 params=1/1 return=bool
OPcache\VolatileCache::has static=0 params=1/1 return=bool
OPcache\VolatileCache::delete static=0 params=1/1 return=bool
OPcache\VolatileCache::deleteMultiple static=0 params=1/1 return=bool
OPcache\VolatileCache::clear static=0 params=0/0 return=bool
OPcache\VolatileCache::lock $lease=int static=0 params=1/2 return=bool
OPcache\VolatileCache::unlock static=0 params=1/1 return=bool
OPcache\VolatileCache::getCacheStoreType static=0 params=1/1 return=OPcache\CacheStoreType
OPcache\VolatileCache::info static=1 params=0/0 return=OPcache\StaticCacheInfo
OPcache\VolatileCache::getCacheStoreTypeByProperty static=1 params=2/2 return=OPcache\CacheStoreType
OPcache\VolatileCache::getCacheStoreTypeByMethod static=1 params=3/3 return=OPcache\CacheStoreType
OPcache\StableCache::getInstance $pool_name=string static=1 params=1/1 return=static
OPcache\StableCache::fetch $default=null|bool|int|float|string|array|object static=0 params=1/2 return=null|bool|int|float|string|array|object
OPcache\StableCache::fetchMultiple $default=?array static=0 params=1/2 return=array|false
OPcache\StableCache::store $value=null|bool|int|float|string|array|object static=0 params=2/2 return=bool
OPcache\StableCache::storeMultiple static=0 params=1/1 return=bool
OPcache\StableCache::storeWithTtl $value=null|bool|int|float|string|array|object $ttl=int static=0 params=3/3 return=bool
OPcache\StableCache::storeMultipleWithTtl $ttl=int static=0 params=2/2 return=bool
OPcache\StableCache::has static=0 params=1/1 return=bool
OPcache\StableCache::delete static=0 params=1/1 return=bool
OPcache\StableCache::deleteMultiple static=0 params=1/1 return=bool
OPcache\StableCache::clear static=0 params=0/0 return=bool
OPcache\StableCache::lock $lease=int static=0 params=1/2 return=bool
OPcache\StableCache::unlock static=0 params=1/1 return=bool
OPcache\StableCache::increment $step=int static=0 params=1/2 return=int|false
OPcache\StableCache::decrement $step=int static=0 params=1/2 return=int|false
OPcache\StableCache::getCacheStoreType static=0 params=1/1 return=OPcache\CacheStoreType
OPcache\StableCache::info static=1 params=0/0 return=OPcache\StaticCacheInfo
OPcache\StableCache::getCacheStoreTypeByProperty static=1 params=2/2 return=OPcache\CacheStoreType
OPcache\StableCache::getCacheStoreTypeByMethod static=1 params=3/3 return=OPcache\CacheStoreType
