--TEST--
UserCache\Cache: negative, sparse, and post-unset integer array keys round-trip exactly
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('array-int-keys');

function ok(string $label, bool $cond): void
{
	echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

$negative = [-5 => 'a', -1 => 'b', 0 => 'c', -100 => 'd', 7 => 'e'];
$cache->store('negative', $negative);
$fetched = $cache->fetch('negative');
ok('negative keys value', $fetched === $negative);
ok('negative keys order', array_keys($fetched) === [-5, -1, 0, -100, 7]);

$sparse = [0 => 'x', 5 => 'y', 10 => 'z'];
$cache->store('sparse', $sparse);
$fetched = $cache->fetch('sparse');
ok('sparse keys value', $fetched === $sparse);
ok('sparse not list', !array_is_list($fetched));

$packed = [10, 20, 30, 40];
unset($packed[1]);
$cache->store('unset', $packed);
$fetched = $cache->fetch('unset');
ok('post-unset keys', array_keys($fetched) === [0, 2, 3]);
ok('post-unset value', $fetched === $packed);
$fetched[] = 99;
ok('append after unset uses key 4', array_key_last($fetched) === 4);

$list = ['p', 'q', 'r'];
$cache->store('list', $list);
$fetched = $cache->fetch('list');
ok('list is list', array_is_list($fetched));
$fetched[] = 's';
ok('list append index 3', array_key_last($fetched) === 3);

$numeric = ['123' => 'a', '007' => 'b', '-3' => 'c', 'x' => 'd'];
$cache->store('numeric', $numeric);
$fetched = $cache->fetch('numeric');
ok('numeric string key value', $fetched === $numeric);
ok('numeric string key coerced', array_key_first($fetched) === 123 && array_key_exists(-3, $fetched));

$limits = [PHP_INT_MAX => 'max', PHP_INT_MIN => 'min', 0 => 'zero'];
$cache->store('limits', $limits);
$fetched = $cache->fetch('limits');
ok('int limit keys', $fetched[PHP_INT_MAX] === 'max' && $fetched[PHP_INT_MIN] === 'min' && $fetched[0] === 'zero');

$transition = [0 => 'a', 1 => 'b'];
$transition[100] = 'c';
$cache->store('transition', $transition);
$fetched = $cache->fetch('transition');
ok('packed to hashed value', $fetched === $transition);
ok('packed to hashed keys', array_keys($fetched) === [0, 1, 100]);

$nested = [
	'neg' => $negative,
	'sparse' => $sparse,
	'list' => ['deep' => [-2 => 'q', 4 => 'r']],
	9 => [PHP_INT_MAX => 'z'],
];
$cache->store('nested', $nested);
ok('nested mixed keys', $cache->fetch('nested') === $nested);

$all = [$negative, $sparse, $packed, $list, $numeric, $limits, $transition, $nested];
$cache->store('all', $all);
ok('serialize parity', serialize($cache->fetch('all')) === serialize($all));
?>
--EXPECT--
negative keys value: OK
negative keys order: OK
sparse keys value: OK
sparse not list: OK
post-unset keys: OK
post-unset value: OK
append after unset uses key 4: OK
list is list: OK
list append index 3: OK
numeric string key value: OK
numeric string key coerced: OK
int limit keys: OK
packed to hashed value: OK
packed to hashed keys: OK
nested mixed keys: OK
serialize parity: OK
