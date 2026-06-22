<?php

#[OPcache\StableStatic]
class StaticCachePreloadGlobalState
{
	public static array $values = ['preload' => true];

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

class StaticCachePreloadMethodState
{
	#[OPcache\VolatileStatic]
	public static function value(): array
	{
		static $value = ['preload' => true];

		return $value;
	}
}
