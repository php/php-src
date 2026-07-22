--TEST--
UserCache\Cache: floats round-trip bit-exactly (negative zero, subnormals, precision)
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('float-precision');

function ok(string $label, bool $cond): void
{
	echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Compare bytes to distinguish negative zero and preserve full precision. */
$floats = [
	'positive zero' => 0.0,
	'negative zero' => -0.0,
	'epsilon' => PHP_FLOAT_EPSILON,
	'float min' => PHP_FLOAT_MIN,
	'float max' => PHP_FLOAT_MAX,
	'smallest subnormal' => 4.9e-324,
	'one third' => 1 / 3,
	'point one plus point two' => 0.1 + 0.2,
	'large' => 1.7e308,
	'small negative' => -2.2250738585072014e-308,
];

foreach ($floats as $label => $value) {
	$cache->store('f', $value);
	$fetched = $cache->fetch('f');
	ok($label, pack('d', $fetched) === pack('d', $value));
}

$cache->store('negzero', -0.0);
ok('negative zero sign bit', fdiv(1.0, $cache->fetch('negzero')) === -INF);

foreach (['inf' => INF, '-inf' => -INF] as $label => $value) {
	$cache->store('nf', $value);
	ok($label, $cache->fetch('nf') === $value);
}
$cache->store('nan', NAN);
ok('nan', is_nan($cache->fetch('nan')));

$graph = [
	'array' => [-0.0, PHP_FLOAT_MIN, 4.9e-324],
	'object' => (object) ['ratio' => 1 / 3, 'neg' => -0.0],
];
$cache->store('graph', $graph);
$fetched = $cache->fetch('graph');
$arrayOk = true;
foreach ($graph['array'] as $index => $value) {
	$arrayOk = $arrayOk && pack('d', $fetched['array'][$index]) === pack('d', $value);
}
ok('floats in array', $arrayOk);
ok('floats in object', pack('d', $fetched['object']->ratio) === pack('d', 1 / 3)
	&& pack('d', $fetched['object']->neg) === pack('d', -0.0));

$cache->store('all', $floats);
ok('serialize parity', serialize($cache->fetch('all')) === serialize($floats));
?>
--EXPECT--
positive zero: OK
negative zero: OK
epsilon: OK
float min: OK
float max: OK
smallest subnormal: OK
one third: OK
point one plus point two: OK
large: OK
small negative: OK
negative zero sign bit: OK
inf: OK
-inf: OK
nan: OK
floats in array: OK
floats in object: OK
serialize parity: OK
