--TEST--
UserCache\Cache: DateTime safe-direct state is restored for subclasses
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
date.timezone=UTC
--FILE--
<?php

class UserCacheCarbonLikeDateTime extends DateTimeImmutable
{
	public string $label = 'default';
}

class UserCacheDateModel
{
	public function __construct(
		public UserCacheCarbonLikeDateTime $createdAt,
		public UserCacheCarbonLikeDateTime $updatedAt,
		public UserCacheCarbonLikeDateTime $deletedAt,
	) {
	}
}

class UserCacheSelfHashDateTime extends DateTime
{
	private string $cachedSelfHash;
	public string $publicSelfHash;

	public function __construct(string $time, DateTimeZone $timezone)
	{
		parent::__construct($time, $timezone);
		$this->cachedSelfHash = spl_object_hash($this);
		$this->publicSelfHash = spl_object_hash($this);
	}

	public function cachedSelfHash(): string
	{
		return $this->cachedSelfHash;
	}
}

class UserCacheMagicDateTime extends DateTime
{
	public static int $serializeCount = 0;
	public static int $unserializeCount = 0;

	private string $label;

	public function __construct(string $time, DateTimeZone $timezone, string $label)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
	}

	public function __serialize(): array
	{
		self::$serializeCount++;

		return parent::__serialize() + ['label' => 'serialized-' . $this->label];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		parent::__unserialize($data);
		$this->label = $data['label'];
	}

	public function label(): string
	{
		return $this->label;
	}
}

class UserCacheMagicSelfHashDateTime extends DateTime
{
	public static int $serializeCount = 0;
	public static int $unserializeCount = 0;

	private string $constructedObjectId;

	public function __construct(string $time, DateTimeZone $timezone)
	{
		parent::__construct($time, $timezone);
		$this->constructedObjectId = spl_object_hash($this);
	}

	public function __serialize(): array
	{
		self::$serializeCount++;

		return parent::__serialize() + ['constructedObjectId' => $this->constructedObjectId];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		parent::__unserialize($data);
		$this->constructedObjectId = spl_object_hash($this);
	}

	public function constructedObjectId(): string
	{
		return $this->constructedObjectId;
	}
}

class UserCacheMagicFilteredDateTime extends DateTime
{
	public static int $serializeCount = 0;
	public static int $unserializeCount = 0;

	public Closure $hidden;
	private string $label;

	public function __construct(string $time, DateTimeZone $timezone, string $label)
	{
		parent::__construct($time, $timezone);
		$this->hidden = static fn(): int => 1;
		$this->label = $label;
	}

	public function __serialize(): array
	{
		self::$serializeCount++;
		$timezone = $this->getTimezone();

		return [
			'date' => $this->format('Y-m-d H:i:s.u'),
			'timezone_type' => 3,
			'timezone' => $timezone->getName(),
			'label' => $this->label,
		];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		parent::__unserialize($data);
		$this->hidden = static fn(): int => 2;
		$this->label = $data['label'];
	}

	public function label(): string
	{
		return $this->label;
	}
}

class UserCacheWakefulDateTime extends DateTime
{
	public static int $sleepCount = 0;
	public static int $wakeupCount = 0;

	private string $label;

	public function __construct(string $time, DateTimeZone $timezone, string $label)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
	}

	public function __sleep(): array
	{
		self::$sleepCount++;

		return ['label'];
	}

	public function __wakeup(): void
	{
		self::$wakeupCount++;
	}

	public function label(): string
	{
		return $this->label;
	}
}

class UserCacheTaggedTimeZone extends DateTimeZone
{
	private string $label;

	public function __construct(string $timezone, string $label)
	{
		parent::__construct($timezone);
		$this->label = $label;
	}

	public function label(): string
	{
		return $this->label;
	}
}

class UserCacheTaggedInterval extends DateInterval
{
	private string $label;
	protected int $revision;

	public function __construct(string $duration, string $label, int $revision)
	{
		parent::__construct($duration);
		$this->label = $label;
		$this->revision = $revision;
	}

	public function describe(): string
	{
		return $this->label . ':' . $this->revision;
	}
}

$cache = UserCache\Cache::getPool('datetime-safe-direct');

$date = new UserCacheCarbonLikeDateTime('2024-01-02 03:04:05.123456', new DateTimeZone('Asia/Tokyo'));
$date->label = 'tokyo';

$model = new UserCacheDateModel(
	new UserCacheCarbonLikeDateTime('2026-06-29 09:00:00.000001', new DateTimeZone('UTC')),
	new UserCacheCarbonLikeDateTime('2026-06-29 09:30:00.000002', new DateTimeZone('Europe/Paris')),
	new UserCacheCarbonLikeDateTime('2026-06-29 10:00:00.000003', new DateTimeZone('America/New_York')),
);

$payload = [
	'date' => $date,
	'model' => $model,
	'offset' => new DateTimeImmutable('2023-10-27 10:00:00.000001 +05:30'),
	'abbr' => new DateTimeImmutable('2023-10-27 10:00:00.000002 EST'),
	'timezone' => new DateTimeZone('Europe/Paris'),
	'interval' => new DateInterval('P1DT2H'),
	'taggedTimezone' => new UserCacheTaggedTimeZone('Europe/Paris', 'paris'),
	'taggedInterval' => new UserCacheTaggedInterval('P1Y2M3DT4H5M6S', 'window', 9),
	'relativeInterval' => DateInterval::createFromDateString('2 days 4 hours'),
	'selfHash' => new UserCacheSelfHashDateTime('2026-06-15 10:15:00.333333', new DateTimeZone('UTC')),
	'magicDate' => new UserCacheMagicDateTime('2026-06-15 10:45:00.654321', new DateTimeZone('UTC'), 'magic'),
	'magicSelfHash' => new UserCacheMagicSelfHashDateTime('2026-06-15 11:45:00.111111', new DateTimeZone('UTC')),
	'magicFiltered' => new UserCacheMagicFilteredDateTime('2026-06-15 12:00:00.222222', new DateTimeZone('UTC'), 'filtered'),
	'wakefulDate' => new UserCacheWakefulDateTime('2026-06-15 12:15:00.987654', new DateTimeZone('UTC'), 'wakeful'),
];

var_dump($cache->store('payload', $payload));

$fetched = $cache->fetch('payload');

var_dump($fetched['date'] instanceof UserCacheCarbonLikeDateTime);
var_dump($fetched['date']->label);
var_dump($fetched['date']->format('Y-m-d H:i:s.u P e'));

var_dump($fetched['model'] instanceof UserCacheDateModel);
var_dump($fetched['model']->createdAt->format('Y-m-d H:i:s.u e'));
var_dump($fetched['model']->updatedAt->format('Y-m-d H:i:s.u e'));
var_dump($fetched['model']->deletedAt->format('Y-m-d H:i:s.u e'));

var_dump($fetched['offset']->format('Y-m-d H:i:s.u P e'));
var_dump($fetched['abbr']->format('Y-m-d H:i:s.u P e'));
var_dump($fetched['timezone']->getName());
var_dump($fetched['interval']->format('%d %h'));
var_dump($fetched['taggedTimezone'] instanceof UserCacheTaggedTimeZone);
var_dump($fetched['taggedTimezone']->getName());
var_dump($fetched['taggedTimezone']->label());
var_dump($fetched['taggedInterval'] instanceof UserCacheTaggedInterval);
var_dump($fetched['taggedInterval']->format('%y-%m-%d %h:%i:%s'));
var_dump($fetched['taggedInterval']->describe());
var_dump($fetched['relativeInterval'] instanceof DateInterval);
var_dump($fetched['relativeInterval']->format('%d %h'));

var_dump($fetched['selfHash'] instanceof UserCacheSelfHashDateTime);
var_dump($fetched['selfHash']->cachedSelfHash() === $payload['selfHash']->cachedSelfHash());
var_dump($fetched['selfHash']->cachedSelfHash() !== spl_object_hash($fetched['selfHash']));
var_dump($fetched['selfHash']->publicSelfHash === spl_object_hash($payload['selfHash']));
var_dump($fetched['selfHash']->publicSelfHash === spl_object_hash($fetched['selfHash']));

var_dump($fetched['magicDate'] instanceof UserCacheMagicDateTime);
var_dump($fetched['magicDate']->format('Y-m-d H:i:s.u e'));
var_dump($fetched['magicDate']->label());
var_dump(UserCacheMagicDateTime::$serializeCount);
var_dump(UserCacheMagicDateTime::$unserializeCount);

var_dump($fetched['magicSelfHash'] instanceof UserCacheMagicSelfHashDateTime);
var_dump($fetched['magicSelfHash']->format('Y-m-d H:i:s.u e'));
var_dump($fetched['magicSelfHash']->constructedObjectId() === spl_object_hash($fetched['magicSelfHash']));
var_dump($fetched['magicSelfHash']->constructedObjectId() !== spl_object_hash($payload['magicSelfHash']));
var_dump(UserCacheMagicSelfHashDateTime::$serializeCount);
var_dump(UserCacheMagicSelfHashDateTime::$unserializeCount);

var_dump($fetched['magicFiltered'] instanceof UserCacheMagicFilteredDateTime);
var_dump($fetched['magicFiltered']->format('Y-m-d H:i:s.u e'));
var_dump($fetched['magicFiltered']->label());
var_dump(($fetched['magicFiltered']->hidden)());
var_dump(UserCacheMagicFilteredDateTime::$serializeCount);
var_dump(UserCacheMagicFilteredDateTime::$unserializeCount);

var_dump($fetched['wakefulDate'] instanceof UserCacheWakefulDateTime);
var_dump($fetched['wakefulDate']->format('Y-m-d H:i:s.u e'));
var_dump($fetched['wakefulDate']->label());
var_dump(UserCacheWakefulDateTime::$sleepCount);
var_dump(UserCacheWakefulDateTime::$wakeupCount);
?>
--EXPECT--
bool(true)
bool(true)
string(5) "tokyo"
string(44) "2024-01-02 03:04:05.123456 +09:00 Asia/Tokyo"
bool(true)
string(30) "2026-06-29 09:00:00.000001 UTC"
string(39) "2026-06-29 09:30:00.000002 Europe/Paris"
string(43) "2026-06-29 10:00:00.000003 America/New_York"
string(40) "2023-10-27 10:00:00.000001 +05:30 +05:30"
string(37) "2023-10-27 10:00:00.000002 -05:00 EST"
string(12) "Europe/Paris"
string(3) "1 2"
bool(true)
string(12) "Europe/Paris"
string(5) "paris"
bool(true)
string(11) "1-2-3 4:5:6"
string(8) "window:9"
bool(true)
string(3) "2 4"
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
string(30) "2026-06-15 10:45:00.654321 UTC"
string(16) "serialized-magic"
int(1)
int(1)
bool(true)
string(30) "2026-06-15 11:45:00.111111 UTC"
bool(true)
bool(true)
int(1)
int(1)
bool(true)
string(30) "2026-06-15 12:00:00.222222 UTC"
string(8) "filtered"
int(2)
int(1)
int(1)
bool(true)
string(30) "2026-06-15 12:15:00.987654 UTC"
string(7) "wakeful"
int(0)
int(0)
