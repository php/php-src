--TEST--
UserCache\Cache: an array stored from several places is stored once and restored as one array
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('shared-array-identity');
$cache->clear();

class UCSharedArrayLeaf
{
	public function __construct(public int $n)
	{
	}
}

/* First line of debug_zval_dump(): the refcount counts every holder plus the argument. */
function uc_array_header(array $outer, string $key): string
{
	ob_start();
	debug_zval_dump($outer[$key]);
	$dump = ob_get_clean();

	return explode("\n", $dump)[0];
}

function uc_check(array $v, string $label): void
{
	$ok = $v['packed_a'][0] === $v['packed_b'][0]
		&& $v['packed_a'][0] === $v['nested']['inner'][0]
		&& $v['packed_a'][0] === $v['by_ref'][0]
		&& $v['packed_a'][0] instanceof UCSharedArrayLeaf
		&& $v['packed_a'][0]->n === 1
		&& $v['packed_a'][1] === 'tail'
		&& $v['shaped_a']['leaf'] === $v['shaped_b']['leaf']
		&& $v['shaped_a']['leaf']->n === 2
		&& $v['shaped_a']['label'] === 'x'
		&& $v['noslot_a'][0] === $v['noslot_b'][0]
		&& $v['noslot_a'][0] instanceof SplFixedArray;

	echo $label, ': ', $ok ? 'ok' : 'FAIL', "\n";
	echo '  packed: ', uc_array_header($v, 'packed_a'), "\n";
	echo '  shaped: ', uc_array_header($v, 'shaped_a'), "\n";
	echo '  noslot: ', uc_array_header($v, 'noslot_a'), "\n";
}

$packed = [new UCSharedArrayLeaf(1), 'tail'];
$shaped = ['leaf' => new UCSharedArrayLeaf(2), 'label' => 'x'];
$noslot = [new SplFixedArray(1), 'y'];

$payload = [
	'packed_a' => $packed,
	'packed_b' => $packed,
	'nested' => ['inner' => $packed],
	'by_ref' => &$packed,
	'shaped_a' => $shaped,
	'shaped_b' => $shaped,
	'noslot_a' => $noslot,
	'noslot_b' => $noslot,
];

var_dump($cache->store('graph', $payload));
unset($payload, $packed, $shaped, $noslot);

uc_check($cache->fetch('graph'), 'fetch 1 (decoded)');
uc_check($cache->fetch('graph'), 'fetch 2 (request-local slot)');
uc_check($cache->fetch('graph'), 'fetch 3 (request-local slot)');

/* The shared array occupies one block: three aliases cost less than three copies. */
$one = [new UCSharedArrayLeaf(3), str_repeat('z', 64)];
$copy1 = $one; $copy1[] = 1; array_pop($copy1);
$copy2 = $one; $copy2[] = 1; array_pop($copy2);

$before = $cache->getPoolStatus()->getUsedMemory();
var_dump($cache->store('shared--', [$one, $one, $one]));
$shared = $cache->getPoolStatus()->getUsedMemory() - $before;

$before = $cache->getPoolStatus()->getUsedMemory();
var_dump($cache->store('distinct', [$one, $copy1, $copy2]));
$distinct = $cache->getPoolStatus()->getUsedMemory() - $before;

echo 'shared aliases use less memory than copies: ';
var_dump($shared > 0 && $shared < $distinct);
?>
--EXPECT--
bool(true)
fetch 1 (decoded): ok
  packed: array(2) packed refcount(5){
  shaped: array(2) refcount(3){
  noslot: array(2) packed refcount(3){
fetch 2 (request-local slot): ok
  packed: array(2) packed refcount(5){
  shaped: array(2) refcount(3){
  noslot: array(2) packed refcount(3){
fetch 3 (request-local slot): ok
  packed: array(2) packed refcount(5){
  shaped: array(2) refcount(3){
  noslot: array(2) packed refcount(3){
bool(true)
bool(true)
shared aliases use less memory than copies: bool(true)
