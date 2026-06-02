--TEST--
OPcache explicit object fetch returns independent request-local object copies
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

final class ExplicitFetchCopyPayload
{
	public function __construct(public string $label) {}
}

final class ExplicitFetchCloneHookPayload
{
	public static int $cloneCount = 0;

	public function __construct(public string $label) {}

	public function __clone()
	{
		self::$cloneCount++;
		$this->label = 'cloned';
	}
}

final class ExplicitFetchDateTimeCloneHookPayload extends DateTime
{
	public static int $cloneCount = 0;

	public string $label = '';

	public function __clone()
	{
		self::$cloneCount++;
		$this->label = 'cloned';
		$this->modify('+10 years');
	}
}

function check_fetch_copies(string $name, callable $store, callable $fetch): void
{
	$store('request_local_copy', new ExplicitFetchCopyPayload($name . '-stored'));

	$first = $fetch('request_local_copy');
	$first->label = $name . '-first-mutated';
	$second = $fetch('request_local_copy');
	var_dump($first === $second);
	echo $name, '-second-after-first-mutate=', $second->label, "\n";

	$second->label = $name . '-mutated';
	echo $name, '-first-after-second-mutate=', $first->label, "\n";

	$third = $fetch('request_local_copy');
	var_dump($first === $third);
	echo $name, '-third=', $third->label, "\n";

	ExplicitFetchCloneHookPayload::$cloneCount = 0;
	$store('request_local_clone_hook', new ExplicitFetchCloneHookPayload($name . '-stored'));
	$hookFirst = $fetch('request_local_clone_hook');
	$hookSecond = $fetch('request_local_clone_hook');
	var_dump($hookFirst === $hookSecond);
	echo $name, '-clone-hook=', ExplicitFetchCloneHookPayload::$cloneCount, ':',
		$hookFirst->label, ':', $hookSecond->label, "\n";

	ExplicitFetchDateTimeCloneHookPayload::$cloneCount = 0;
	$date = new ExplicitFetchDateTimeCloneHookPayload('2026-05-15 12:34:56', new DateTimeZone('Asia/Tokyo'));
	$date->label = $name . '-stored';
	$store('request_local_datetime_clone_hook', $date);
	$dateFirst = $fetch('request_local_datetime_clone_hook');
	$dateSecond = $fetch('request_local_datetime_clone_hook');
	var_dump($dateFirst === $dateSecond);
	echo $name, '-datetime-clone-hook=', ExplicitFetchDateTimeCloneHookPayload::$cloneCount, ':',
		$dateFirst->label, ':', $dateSecond->label, "\n";
	echo $name, '-datetime-second-before=', $dateSecond->format(DateTimeInterface::ATOM), "\n";

	$dateFirst->modify('+1 day');
	$dateFirst->label = $name . '-datetime-first-mutated';
	echo $name, '-datetime-second-after-first-mutate=', $dateSecond->format(DateTimeInterface::ATOM), ':',
		$dateSecond->label, "\n";

	$dateSecond->modify('+2 days');
	$dateSecond->label = $name . '-datetime-second-mutated';
	echo $name, '-datetime-first-after-second-mutate=', $dateFirst->format(DateTimeInterface::ATOM), ':',
		$dateFirst->label, "\n";

	$dateThird = $fetch('request_local_datetime_clone_hook');
	var_dump($dateFirst === $dateThird);
	echo $name, '-datetime-third=', $dateThird->format(DateTimeInterface::ATOM), ':',
		$dateThird->label, "\n";
}

OPcache\VolatileCache::clear();
OPcache\PinnedCache::clear();

check_fetch_copies(
	'volatile',
	static fn (string $key, mixed $payload): bool => OPcache\VolatileCache::set($key, $payload),
	static fn (string $key): mixed => OPcache\VolatileCache::get($key),
);

check_fetch_copies(
	'pinned',
	static function (string $key, mixed $payload): bool {
		OPcache\PinnedCache::set($key, $payload);

		return true;
	},
	static fn (string $key): mixed => OPcache\PinnedCache::get($key),
);

?>
--EXPECT--
bool(false)
volatile-second-after-first-mutate=volatile-stored
volatile-first-after-second-mutate=volatile-first-mutated
bool(false)
volatile-third=volatile-stored
bool(false)
volatile-clone-hook=0:volatile-stored:volatile-stored
bool(false)
volatile-datetime-clone-hook=0:volatile-stored:volatile-stored
volatile-datetime-second-before=2026-05-15T12:34:56+09:00
volatile-datetime-second-after-first-mutate=2026-05-15T12:34:56+09:00:volatile-stored
volatile-datetime-first-after-second-mutate=2026-05-16T12:34:56+09:00:volatile-datetime-first-mutated
bool(false)
volatile-datetime-third=2026-05-15T12:34:56+09:00:volatile-stored
bool(false)
pinned-second-after-first-mutate=pinned-stored
pinned-first-after-second-mutate=pinned-first-mutated
bool(false)
pinned-third=pinned-stored
bool(false)
pinned-clone-hook=0:pinned-stored:pinned-stored
bool(false)
pinned-datetime-clone-hook=0:pinned-stored:pinned-stored
pinned-datetime-second-before=2026-05-15T12:34:56+09:00
pinned-datetime-second-after-first-mutate=2026-05-15T12:34:56+09:00:pinned-stored
pinned-datetime-first-after-second-mutate=2026-05-16T12:34:56+09:00:pinned-datetime-first-mutated
bool(false)
pinned-datetime-third=2026-05-15T12:34:56+09:00:pinned-stored
