--TEST--
UserCache scalar overwrite stripes preserve types, overflow, TTL and fallback operations
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('scalar-stripes');
$cache->clear();
function check(bool $condition): void {
    if (!$condition) throw new RuntimeException('unexpected result');
}
$cache->store('value', 0);
$cache->store('value', 1); // The first eligible overwrite enables striped writes.
foreach ([null, false, true, 17, -42, 1.25, PHP_INT_MAX, PHP_INT_MIN] as $value) {
    check($cache->store('value', $value));
    check($cache->fetch('value') === $value);
    check($cache->fetchMultiple(['value'])['value'] === $value);
}
$cache->store('value', PHP_INT_MAX);
try { $cache->increment('value'); } catch (ArithmeticError $e) { echo "overflow\n"; }
check($cache->fetch('value') === PHP_INT_MAX);
$cache->store('value', false);
try { $cache->decrement('value'); } catch (ValueError $e) { echo "type\n"; }
check($cache->fetch('value') === false);
$cache->store('value', 40);
check($cache->increment('value', 3) === 43);
check($cache->decrement('value', 2) === 41);
check($cache->store('value', 9, 3600)); // TTL uses the ordinary writer.
check($cache->increment('value') === 10);
check($cache->store('value', ['nested' => [1, 2]]));
check($cache->fetch('value') === ['nested' => [1, 2]]);
check($cache->store('value', 10));
check($cache->storeMultiple(['value' => 11, 'other' => 12]));
check($cache->fetchMultiple(['value', 'other']) === ['value' => 11, 'other' => 12]);
check($cache->delete('value'));
check($cache->increment('value', 5) === 5);
echo "OK\n";
?>
--EXPECT--
overflow
type
OK
