--TEST--
UserCache\Cache: storable values use shared graph and opaque values are refused
--EXTENSIONS--
spl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
date.timezone=UTC
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
$cache = UserCache\Cache::getPool('storable-values');

enum UserCacheStorableSuit: string
{
	case Hearts = 'h';
}

class UserCacheStorablePlain
{
	public int $x = 5;
}

class UserCacheStorableSleep
{
	public static int $sleepCalls = 0;
	public static int $wakeupCalls = 0;

	public int $a = 1;
	public int $b = 2;

	public function __sleep(): array
	{
		self::$sleepCalls++;
		return ['a'];
	}

	public function __wakeup(): void
	{
		self::$wakeupCalls++;
	}
}

class UserCacheUnsupportedBox
{
	public function __construct(public mixed $value)
	{
	}
}

function user_cache_storable_tier(UserCache\Cache $cache, mixed $value): string
{
	static $i = 0;

	$i++;
	try {
		return $cache->store('storable_' . $i, $value) ? 'stored' : 'REFUSED';
	} catch (TypeError $e) {
		return 'TypeError';
	}
}

echo "array:        ", user_cache_storable_tier($cache, ['a' => 1, 'b' => [2, 3]]), "\n";
echo "stdClass:     ", user_cache_storable_tier($cache, (object) ['x' => 1, 'y' => [2]]), "\n";
echo "json object:  ", user_cache_storable_tier($cache, json_decode('{"a":1,"b":{"c":2}}')), "\n";
echo "plain object: ", user_cache_storable_tier($cache, new UserCacheStorablePlain()), "\n";
echo "DateTime:     ", user_cache_storable_tier($cache, new DateTime('2026-01-01')), "\n";
echo "DateInterval: ", user_cache_storable_tier($cache, new DateInterval('P1D')), "\n";
echo "ArrayObject:  ", user_cache_storable_tier($cache, new ArrayObject([1, 2, 3])), "\n";
echo "SplStack:     ", user_cache_storable_tier($cache, (function () { $s = new SplStack(); $s->push(1); return $s; })()), "\n";
echo "__sleep obj:  ", user_cache_storable_tier($cache, new UserCacheStorableSleep()), "\n";
echo "enum:         ", user_cache_storable_tier($cache, UserCacheStorableSuit::Hearts), "\n";
$ref = 1;
echo "array w/ ref: ", user_cache_storable_tier($cache, ['x' => &$ref, 'y' => &$ref]), "\n";

$sleepFetched = $cache->fetch('storable_9');
echo "__sleep magic calls: ";
var_dump([UserCacheStorableSleep::$sleepCalls, UserCacheStorableSleep::$wakeupCalls]);
echo "__sleep object access: ";
var_dump($sleepFetched->a . ':' . $sleepFetched->b);

$resource = fopen(__FILE__, 'r');
$closure = static fn () => true;

try {
	$cache->store('resource-root', $resource);
	echo "Resource:      stored\n";
} catch (TypeError $e) {
	echo "Resource:      TypeError\n";
}

try {
	$cache->store('closure-root', $closure);
	echo "Closure:      stored\n";
} catch (TypeError $e) {
	echo "Closure:      TypeError\n";
}

echo "nested resource: ", user_cache_storable_tier($cache, ['value' => $resource]), "\n";
echo "nested closure:  ", user_cache_storable_tier($cache, ['value' => $closure]), "\n";
echo "object resource: ", user_cache_storable_tier($cache, new UserCacheUnsupportedBox($resource)), "\n";
echo "object closure:  ", user_cache_storable_tier($cache, new UserCacheUnsupportedBox($closure)), "\n";
echo "fixed resource:  ", user_cache_storable_tier($cache, SplFixedArray::fromArray([$resource], false)), "\n";
echo "fixed closure:   ", user_cache_storable_tier($cache, SplFixedArray::fromArray([$closure], false)), "\n";
echo "array resource:  ", user_cache_storable_tier($cache, new ArrayObject(['value' => $resource])), "\n";
echo "array closure:   ", user_cache_storable_tier($cache, new ArrayObject(['value' => $closure])), "\n";

$fiber = new Fiber(function () { Fiber::suspend(); });
$fiber->start();
echo "Fiber:        ", user_cache_storable_tier($cache, $fiber), "\n";
echo "Generator:    ", user_cache_storable_tier($cache, (function () { yield 1; })()), "\n";
echo "WeakMap:      ", user_cache_storable_tier($cache, (function () { $m = new WeakMap(); $m[new stdClass()] = 1; return $m; })()), "\n";

fclose($resource);
?>
--EXPECT--
array:        stored
stdClass:     stored
json object:  stored
plain object: stored
DateTime:     stored
DateInterval: stored
ArrayObject:  stored
SplStack:     stored
__sleep obj:  stored
enum:         stored
array w/ ref: stored
__sleep magic calls: array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}
__sleep object access: string(3) "1:2"
Resource:      TypeError
Closure:      TypeError
nested resource: TypeError
nested closure:  TypeError
object resource: TypeError
object closure:  TypeError
fixed resource:  TypeError
fixed closure:   TypeError
array resource:  TypeError
array closure:   TypeError
Fiber:        TypeError
Generator:    TypeError
WeakMap:      TypeError
