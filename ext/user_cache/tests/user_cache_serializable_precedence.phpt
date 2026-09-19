--TEST--
UserCache\Cache: Serializable interface outranks __sleep/__wakeup/__unserialize (native precedence)
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
$cache = UserCache\Cache::getPool('serializable-precedence');

function ok(string $label, bool $cond): void
{
	echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Serializable takes precedence over conflicting legacy magic methods. */

class PrecedenceSerVsSleep implements Serializable
{
	public int $kept = 0;
	public string $via = 'none';

	public function serialize(): string
	{
		return (string) $this->kept;
	}

	public function unserialize(string $data): void
	{
		$this->kept = (int) $data;
		$this->via = 'iface';
	}

	public function __sleep(): array
	{
		return ['kept'];
	}

	public function __wakeup(): void
	{
		$this->via = 'sleep';
	}
}

class PrecedenceSerVsWakeup implements Serializable
{
	public int $kept = 0;
	public string $via = 'none';

	public function serialize(): string
	{
		return (string) $this->kept;
	}

	public function unserialize(string $data): void
	{
		$this->kept = (int) $data;
		$this->via = 'iface';
	}

	public function __wakeup(): void
	{
		$this->via = 'wakeup';
	}
}

class PrecedenceSerVsMagicUnserialize implements Serializable
{
	public int $kept = 0;
	public string $via = 'none';

	public function serialize(): string
	{
		return (string) $this->kept;
	}

	public function unserialize(string $data): void
	{
		$this->kept = (int) $data;
		$this->via = 'iface';
	}

	public function __unserialize(array $data): void
	{
		$this->via = 'magic';
	}
}

function check_precedence(UserCache\Cache $cache, string $label, object $value): void
{
	$native = unserialize(serialize($value));
	$cache->store($label, $value);
	$fetched = $cache->fetch($label);

	ok($label . ' instanceof', $fetched instanceof $value);
	ok($label . ' interface won (native parity)',
		$fetched->via === $native->via && $fetched->via === 'iface');
	ok($label . ' value', $fetched->kept === $native->kept);
}

$a = new PrecedenceSerVsSleep();
$a->kept = 5;
check_precedence($cache, 'PrecedenceSerVsSleep', $a);

$b = new PrecedenceSerVsWakeup();
$b->kept = 6;
check_precedence($cache, 'PrecedenceSerVsWakeup', $b);

$c = new PrecedenceSerVsMagicUnserialize();
$c->kept = 7;
check_precedence($cache, 'PrecedenceSerVsMagicUnserialize', $c);

/* __serialize()/__unserialize() take precedence over Serializable. */
class PrecedenceMagicOverIface implements Serializable
{
	public int $kept = 0;
	public string $via = 'none';

	public function serialize(): string
	{
		return 'IFACE';
	}

	public function unserialize(string $data): void
	{
		$this->via = 'iface';
	}

	public function __serialize(): array
	{
		return ['kept' => $this->kept];
	}

	public function __unserialize(array $data): void
	{
		$this->kept = $data['kept'];
		$this->via = 'magic';
	}
}

$d = new PrecedenceMagicOverIface();
$d->kept = 8;
$native = unserialize(serialize($d));
$cache->store('magic-over-iface', $d);
$fetched = $cache->fetch('magic-over-iface');
ok('magic outranks interface', $fetched->via === $native->via && $fetched->via === 'magic');
ok('magic value', $fetched->kept === $native->kept && $fetched->kept === 8);
?>
--EXPECT--
PrecedenceSerVsSleep instanceof: OK
PrecedenceSerVsSleep interface won (native parity): OK
PrecedenceSerVsSleep value: OK
PrecedenceSerVsWakeup instanceof: OK
PrecedenceSerVsWakeup interface won (native parity): OK
PrecedenceSerVsWakeup value: OK
PrecedenceSerVsMagicUnserialize instanceof: OK
PrecedenceSerVsMagicUnserialize interface won (native parity): OK
PrecedenceSerVsMagicUnserialize value: OK
magic outranks interface: OK
magic value: OK
