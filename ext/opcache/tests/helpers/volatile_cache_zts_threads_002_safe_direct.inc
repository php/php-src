<?php

class EventDateTime extends DateTime
{
	private string $label;
	protected int $revision;

	public function __construct(string $time, DateTimeZone $timezone, string $label, int $revision)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
		$this->revision = $revision;
	}

	public function describe(): string
	{
		return $this->label . ':' . $this->revision;
	}
}

class LabelIterator extends ArrayIterator
{
}

class TaggedCollection extends ArrayObject
{
	private string $type;

	public function __construct(array $data, string $type, string $iteratorClass)
	{
		parent::__construct($data, 0, $iteratorClass);
		$this->type = $type;
	}

	public function type(): string
	{
		return $this->type;
	}
}

class CustomSerializedDateTime extends DateTime
{
	public static int $unserializeCount = 0;

	private string $label;

	public function __construct(string $time, DateTimeZone $timezone, string $label)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
	}

	public function __serialize(): array
	{
		return parent::__serialize() + ['label' => $this->label];
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

if ($mode === 'store') {
	return OPcache\volatile_store(
		'zts_safe_direct_event_datetime',
		new EventDateTime('2026-06-15 09:30:00.123456', new DateTimeZone('Europe/Paris'), 'launch', 7),
	) && OPcache\volatile_store(
		'zts_safe_direct_tagged_collection',
		new TaggedCollection(['alpha' => 10, 'beta' => 20], 'metric', LabelIterator::class),
	) && OPcache\volatile_store(
		'zts_safe_direct_custom_datetime',
		new CustomSerializedDateTime('2026-06-15 10:45:00.654321', new DateTimeZone('UTC'), 'fallback'),
	);
}

$event = OPcache\volatile_fetch('zts_safe_direct_event_datetime');
$collection = OPcache\volatile_fetch('zts_safe_direct_tagged_collection');
$fallback = OPcache\volatile_fetch('zts_safe_direct_custom_datetime');

if (!$event instanceof EventDateTime) {
	return false;
}

if (!$collection instanceof TaggedCollection) {
	return false;
}

if (!$fallback instanceof CustomSerializedDateTime) {
	return false;
}

$iterator = $collection->getIterator();

return $event->format('Y-m-d H:i:s.u e') === '2026-06-15 09:30:00.123456 Europe/Paris'
	&& $event->describe() === 'launch:7'
	&& $collection->type() === 'metric'
	&& $iterator instanceof LabelIterator
	&& $collection['alpha'] === 10
	&& $collection['beta'] === 20
	&& $fallback->format('Y-m-d H:i:s.u e') === '2026-06-15 10:45:00.654321 UTC'
	&& $fallback->label() === 'fallback'
	&& CustomSerializedDateTime::$unserializeCount === 1;
