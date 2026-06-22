--TEST--
OPcache class-level static cache attributes are not inherited
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/stable_static_inherited_attributes_001.php', <<<'PHP'
<?php

#[OPcache\StableStatic]
class StaticCacheAttributeParent
{
	public static int $parentValue = 0;

	public static function parentNext(): int
	{
		static $value = 0;

		return ++$value;
	}
}

class StaticCacheAttributeChild extends StaticCacheAttributeParent
{
	public static int $childValue = 0;

	public static function childNext(): int
	{
		static $value = 0;

		return ++$value;
	}
}

echo ++StaticCacheAttributeParent::$parentValue, ',', StaticCacheAttributeParent::parentNext(), ',';
echo ++StaticCacheAttributeChild::$childValue, ',', StaticCacheAttributeChild::childNext(), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.stable_size_mb=32 -d opcache.file_update_protection=0');

$url = 'http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_inherited_attributes_001.php';
echo file_get_contents($url);
echo file_get_contents($url);

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_static_inherited_attributes_001.php');
?>
--EXPECT--
1,1,1,1
2,2,1,1
