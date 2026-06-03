<?php
#[OPcache\StableStatic]
class ZtsRaceClassState
{
	public static array $data = ['value' => 0, 'numbers' => []];

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

class ZtsRacePropertyState
{
	#[OPcache\StableStatic]
	public static array $data = ['value' => 0, 'numbers' => []];
}

class ZtsRaceMethodState
{
	#[OPcache\StableStatic]
	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

function zts_stable_static_wait_for_file(string $path): bool
{
	$deadline = microtime(true) + 5.0;
	while (!file_exists($path)) {
		if (microtime(true) >= $deadline) {
			return false;
		}
		usleep(1000);
	}
	return true;
}

function zts_stable_static_tuple(): string
{
	return ZtsRaceClassState::$data['value'] . ','
		. array_sum(ZtsRaceClassState::$data['numbers']) . ','
		. ZtsRaceClassState::next() . ','
		. ZtsRacePropertyState::$data['value'] . ','
		. array_sum(ZtsRacePropertyState::$data['numbers']) . ','
		. ZtsRaceMethodState::next();
}

$readyFile = __FILE__ . '.ready';
$doneFile = __FILE__ . '.done';

if ($mode === 'init') {
	@unlink($readyFile);
	@unlink($doneFile);
	opcache_reset();
	ZtsRaceClassState::$data = ['value' => 1, 'numbers' => [1]];
	ZtsRaceClassState::next();
	ZtsRacePropertyState::$data = ['value' => 1, 'numbers' => [1]];
	ZtsRaceMethodState::next();
	return true;
}

if ($mode === 'writer') {
	if (!zts_stable_static_wait_for_file($readyFile)) {
		return false;
	}
	$classMethod = ZtsRaceClassState::next();
	ZtsRaceClassState::$data['value'] = 10;
	ZtsRaceClassState::$data['numbers'][] = 20;
	ZtsRacePropertyState::$data['value'] = 10;
	ZtsRacePropertyState::$data['numbers'][] = 20;
	$method = ZtsRaceMethodState::next();
	return $classMethod === 2
		&& $method === 2
		&& ZtsRaceClassState::$data['value'] === 10
		&& array_sum(ZtsRaceClassState::$data['numbers']) === 21
		&& ZtsRacePropertyState::$data['value'] === 10
		&& array_sum(ZtsRacePropertyState::$data['numbers']) === 21;
}

if ($mode === 'reader') {
	file_put_contents($readyFile, 'ready');
	if (!zts_stable_static_wait_for_file($doneFile)) {
		return false;
	}
	return zts_stable_static_tuple() === '10,21,3,1,1,3';
}

if ($mode === 'verify') {
	@unlink($readyFile);
	@unlink($doneFile);
	return zts_stable_static_tuple() === '10,21,4,1,1,4';
}

return false;
