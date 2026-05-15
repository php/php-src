<?php

class LargePayload
{
	public function __construct(
		public array $rows,
		public string $label,
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
	return OPcache\volatile_store('zts_materialization_plain_payload', new LargePayload(build_rows(), 'plain'));
}

$before = memory_get_usage();
$fetched = OPcache\volatile_fetch('zts_materialization_plain_payload');

if (!$fetched instanceof LargePayload) {
	return false;
}

$readOnly = $fetched->rows[100]['text'];
$afterFetch = memory_get_usage();
$fetched->rows[100]['text'] = 'changed';
$afterMutate = memory_get_usage();

return $readOnly === str_repeat(chr(65 + (100 % 26)), 96)
	&& ($afterFetch - $before) < 262144
	&& ($afterMutate - $afterFetch) > 131072;
