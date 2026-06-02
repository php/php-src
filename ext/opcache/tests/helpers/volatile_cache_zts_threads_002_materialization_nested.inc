<?php

class LargePayload
{
	public function __construct(
		public array $rows,
		public string $label,
	) {}
}

class WrappedPayload
{
	public function __construct(
		public LargePayload $payload,
		public DateTimeImmutable $timestamp,
	) {}
}

function build_rows(): array
{
	$rows = [];

	for ($i = 0; $i < 12000; $i++) {
		$rows[] = [
			'id' => $i,
			'text' => str_repeat(chr(65 + ($i % 26)), 96),
			'flags' => [$i, $i + 1, $i + 2, $i + 3],
		];
	}

	return $rows;
}

if ($mode === 'store') {
	return OPcache\VolatileCache::set(
		'zts_materialization_nested_payload',
		new WrappedPayload(
			new LargePayload(build_rows(), 'nested'),
			new DateTimeImmutable('2026-06-15 09:30:00', new DateTimeZone('UTC')),
		),
	);
}

$before = memory_get_usage();
$fetched = OPcache\VolatileCache::get('zts_materialization_nested_payload');

if (!$fetched instanceof WrappedPayload) {
	return false;
}

$readOnly = $fetched->timestamp->format(DateTimeInterface::ATOM) . '|' . $fetched->payload->rows[100]['text'];
$afterFetch = memory_get_usage();
$fetched->payload->rows[100]['text'] = 'changed';
$afterMutate = memory_get_usage();

return $readOnly === '2026-06-15T09:30:00+00:00|' . str_repeat(chr(65 + (100 % 26)), 96)
	&& ($afterFetch - $before) < 262144
	&& ($afterMutate - $afterFetch) > 131072;
