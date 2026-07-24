--TEST--
UserCache\Cache: request-local slots are promoted after the first shared-graph fetch
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php
$cache = UserCache\Cache::getPool('deferred-promotion');
$cache->clear();

class UserCacheDeferredPromotionDate extends DateTime
{
	private string $cachedSelfHash;

	public function __construct(string $time, DateTimeZone $timezone)
	{
		parent::__construct($time, $timezone);
		$this->cachedSelfHash = spl_object_hash($this);
	}

	public function cachedSelfHash(): string
	{
		return $this->cachedSelfHash;
	}
}

$pid = pcntl_fork();
if ($pid === 0) {
	$date = new UserCacheDeferredPromotionDate('2026-07-03 12:34:56.123456', new DateTimeZone('Asia/Tokyo'));
	$payload = [
		'first' => $date,
		'second' => $date,
		'nested' => ['label' => 'original'],
		'storedHash' => $date->cachedSelfHash(),
	];

	var_dump($cache->store('payload', $payload));
	exit(0);
}

if ($pid < 0) {
	echo "pcntl_fork() failed\n";
	exit(1);
}

pcntl_waitpid($pid, $status);

$first = $cache->fetch('payload');
$first['first']->modify('+1 day');
$first['nested']['label'] = 'mutated-first';

$second = $cache->fetch('payload');
$secondDateUnchanged = $second['first']->format('Y-m-d H:i:s.u e') === '2026-07-03 12:34:56.123456 Asia/Tokyo';
$secondNestedUnchanged = $second['nested']['label'] === 'original';
$second['first']->modify('+2 days');
$second['nested']['label'] = 'mutated-second';

$third = $cache->fetch('payload');

echo "status: ", pcntl_wexitstatus($status), "\n";
echo "first alias: ";
var_dump($first['first'] === $first['second']);
echo "second alias: ";
var_dump($second['first'] === $second['second']);
echo "third alias: ";
var_dump($third['first'] === $third['second']);
echo "fetches independent: ";
var_dump($first['first'] !== $second['first'] && $second['first'] !== $third['first']);
echo "second unchanged by first: ";
var_dump($secondDateUnchanged);
var_dump($secondNestedUnchanged);
echo "third unchanged by second: ";
var_dump($third['first']->format('Y-m-d H:i:s.u e') === '2026-07-03 12:34:56.123456 Asia/Tokyo');
var_dump($third['nested']['label'] === 'original');
echo "object hashes preserved as stored: ";
var_dump($first['first']->cachedSelfHash() === $first['storedHash']);
var_dump($second['first']->cachedSelfHash() === $second['storedHash']);
var_dump($third['first']->cachedSelfHash() === $third['storedHash']);
?>
--EXPECTF--
bool(true)
status: 0
first alias: bool(true)
second alias: bool(true)
third alias: bool(true)
fetches independent: bool(true)
second unchanged by first: bool(true)
bool(true)
third unchanged by second: bool(true)
bool(true)
object hashes preserved as stored: bool(true)
bool(true)
bool(true)
