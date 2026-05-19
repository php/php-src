--TEST--
OPcache PinnedStatic and explicit static caches are deleted by opcache_reset()
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

$subject = __DIR__ . '/pinned_static_reset_001_subject.php';
$resetter = __DIR__ . '/pinned_static_reset_001_resetter.php';

file_put_contents($subject, <<<'PHP'
<?php
#[OPcache\PinnedStatic]
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
	#[OPcache\PinnedStatic]
	public static int $value = 0;
}

class ResetMethodState
{
	#[OPcache\PinnedStatic]
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
OPcache\volatile_store('pinned_static_reset_explicit', 'keep');
OPcache\pinned_store('pinned_static_reset_pinned_explicit', 'keep-pinned');
var_dump(opcache_reset());
echo OPcache\volatile_fetch('pinned_static_reset_explicit', 'missing-volatile'), "\n";
echo OPcache\pinned_fetch('pinned_static_reset_pinned_explicit', 'missing-pinned'), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_reset_001_subject.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_reset_001_subject.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_reset_001_resetter.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_reset_001_subject.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_reset_001_subject.php');
@unlink(__DIR__ . '/pinned_static_reset_001_resetter.php');
?>
--EXPECT--
1,1,1,1
2,2,2,2
bool(true)
missing-volatile
missing-pinned
1,1,1,1
