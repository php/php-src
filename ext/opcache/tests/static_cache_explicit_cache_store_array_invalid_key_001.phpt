--TEST--
OPcache explicit store_array APIs reject non-string keys before storing any entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.persistent_size_mb=32
--FILE--
<?php

OPcache\volatile_clear();
OPcache\persistent_clear();

foreach ([
	'volatile' => ['OPcache\\volatile_store_array', 'OPcache\\volatile_fetch'],
	'persistent' => ['OPcache\\persistent_store_array', 'OPcache\\persistent_fetch'],
] as $label => [$storeArray, $fetch]) {
	$key = $label . '_first';

	try {
		$storeArray([
			$key => 'stored',
			0 => 'bad',
		]);
	} catch (ValueError $exception) {
		echo $label, ': ', $exception->getMessage(), "\n";
	}

	var_dump($fetch($key, 'missing'));
}

?>
--EXPECT--
volatile: OPcache\volatile_store_array(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
string(7) "missing"
persistent: OPcache\persistent_store_array(): Argument #1 ($values) must be an array with non-empty string keys that are not reserved static-cache class keys or loaded class names
string(7) "missing"
