--TEST--
OPcache StableStatic and explicit static caches are deleted by opcache_reset()
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

$subject = __DIR__ . '/stable_static_reset_001_subject.php';
$resetter = __DIR__ . '/stable_static_reset_001_resetter.php';

file_put_contents($subject, <<<'PHP'
<?php
#[OPcache\StableStatic]
class ResetClassState
{
	public static int $value = 0;

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

class ResetPropertyState
{
	#[OPcache\StableStatic]
	public static int $value = 0;
}

class ResetMethodState
{
	#[OPcache\StableStatic]
	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

echo ++ResetClassState::$value, ',', ResetClassState::next(), ',', ++ResetPropertyState::$value, ',', ResetMethodState::next(), "\n";
PHP);

file_put_contents($resetter, <<<'PHP'
<?php
OPcache\VolatileCache::getInstance('default')->store('stable_static_reset_explicit', 'keep');
OPcache\StableCache::getInstance('default')->store('stable_static_reset_stable_explicit', 'keep-stable');
var_dump(opcache_reset());
echo OPcache\VolatileCache::getInstance('default')->fetch('stable_static_reset_explicit', 'missing-volatile'), "\n";
echo OPcache\StableCache::getInstance('default')->fetch('stable_static_reset_stable_explicit', 'missing-stable'), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.stable_size_mb=32 -d opcache.file_update_protection=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_reset_001_subject.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_reset_001_subject.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_reset_001_resetter.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_reset_001_subject.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_static_reset_001_subject.php');
@unlink(__DIR__ . '/stable_static_reset_001_resetter.php');
?>
--EXPECT--
1,1,1,1
2,2,2,2
bool(true)
missing-volatile
missing-stable
1,1,1,1
