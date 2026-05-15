--TEST--
OPcache PersistentStatic state is deleted only for the invalidated script
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

$subject = __DIR__ . '/persistent_static_invalidate_001_subject.php';
$other = __DIR__ . '/persistent_static_invalidate_001_other.php';
$invalidator = __DIR__ . '/persistent_static_invalidate_001_invalidator.php';

file_put_contents($subject, <<<'PHP'
<?php
#[OPcache\PersistentStatic]
class InvalidateClassState
{
	public static int $value = 0;

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

class InvalidatePropertyState
{
	#[OPcache\PersistentStatic]
	public static int $value = 0;
}

class InvalidateMethodState
{
	#[OPcache\PersistentStatic]
	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

echo ++InvalidateClassState::$value, ',', InvalidateClassState::next(), ',', ++InvalidatePropertyState::$value, ',', InvalidateMethodState::next(), "\n";
PHP);

file_put_contents($other, <<<'PHP'
<?php
#[OPcache\PersistentStatic]
class InvalidateOtherClassState
{
	public static int $value = 0;
}

echo ++InvalidateOtherClassState::$value, "\n";
PHP);

file_put_contents($invalidator, <<<'PHP'
<?php
OPcache\volatile_store('persistent_static_invalidate_explicit', 'keep');
OPcache\persistent_store('persistent_static_invalidate_persistent_explicit', 'keep-persistent');
var_dump(opcache_invalidate(__DIR__ . '/persistent_static_invalidate_001_subject.php', true));
echo OPcache\volatile_fetch('persistent_static_invalidate_explicit', 'missing-volatile'), "\n";
echo OPcache\persistent_fetch('persistent_static_invalidate_persistent_explicit', 'missing-persistent'), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.persistent_size_mb=32 -d opcache.file_update_protection=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_invalidate_001_subject.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_invalidate_001_subject.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_invalidate_001_other.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_invalidate_001_other.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_invalidate_001_invalidator.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_invalidate_001_subject.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_invalidate_001_other.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/persistent_static_invalidate_001_subject.php');
@unlink(__DIR__ . '/persistent_static_invalidate_001_other.php');
@unlink(__DIR__ . '/persistent_static_invalidate_001_invalidator.php');
?>
--EXPECT--
1,1,1,1
2,2,2,2
1
2
bool(true)
keep
keep-persistent
1,1,1,1
3
