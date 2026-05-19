--TEST--
OPcache static cache attribute signatures expose TTL and strategy
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
		return $type->getName();
	}

	return 'complex';
}

$volatile = new ReflectionClass(OPcache\VolatileStatic::class);
$constructor = $volatile->getConstructor();
foreach ($constructor->getParameters() as $parameter) {
	$default = $parameter->isDefaultValueAvailable() ? $parameter->getDefaultValue() : null;
	if ($default instanceof UnitEnum) {
		$default = $default::class . '::' . $default->name;
	}
	echo $parameter->getName(), ':', describe_type($parameter->getType()), ':', var_export($default, true), "\n";
}

$attribute = new OPcache\VolatileStatic(5, OPcache\CacheStrategy::Tracking);
var_dump($attribute->ttl);
var_dump($attribute->strategy === OPcache\CacheStrategy::Tracking);

try {
	$attribute->ttl = 1;
} catch (Error $exception) {
	echo "readonly-ttl\n";
}

var_dump((new ReflectionClass(OPcache\PinnedStatic::class))->getConstructor());

?>
--EXPECT--
ttl:int:0
strategy:OPcache\CacheStrategy:'OPcache\\CacheStrategy::Immediate'
int(5)
bool(true)
readonly-ttl
NULL
