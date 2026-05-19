--TEST--
OPcache VolatileStatic tracking skips read-only shutdown publish like PinnedStatic
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_static_readonly_publish_001.php', <<<'PHP'
<?php
class ReadOnlyPublishProbe
{
	public function __construct(
		private string $logFile,
		public int $value,
	) {
	}

	public function __serialize(): array
	{
		file_put_contents($this->logFile, "serialize\n", FILE_APPEND);
		return ['logFile' => $this->logFile, 'value' => $this->value];
	}

	public function __unserialize(array $data): void
	{
		$this->logFile = $data['logFile'];
		$this->value = $data['value'];
	}
}

class CachedReadOnlyMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(string $logFile): int
	{
		static $probe = null;

		if ($probe === null) {
			$probe = new ReadOnlyPublishProbe($logFile, 11);
		}

		return $probe->value;
	}
}

class PinnedReadOnlyMethodState
{
	#[OPcache\PinnedStatic]
	public static function value(string $logFile): int
	{
		static $probe = null;

		if ($probe === null) {
			$probe = new ReadOnlyPublishProbe($logFile, 13);
		}

		return $probe->value;
	}
}

class CachedReadOnlyPropertyState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?ReadOnlyPublishProbe $probe = null;

	public static function value(string $logFile): int
	{
		self::$probe ??= new ReadOnlyPublishProbe($logFile, 17);
		return self::$probe->value;
	}
}

class PinnedReadOnlyPropertyState
{
	#[OPcache\PinnedStatic]
	public static ?ReadOnlyPublishProbe $probe = null;

	public static function value(string $logFile): int
	{
		self::$probe ??= new ReadOnlyPublishProbe($logFile, 19);
		return self::$probe->value;
	}
}

function readonly_publish_log_file(string $kind): string
{
	return __DIR__ . '/pinned_static_readonly_publish_001_' . $kind . '.log';
}

$kinds = [
	'cached_method',
	'cached_property',
	'pinned_method',
	'pinned_property',
];
$action = $_GET['action'] ?? 'value';
$kind = $_GET['kind'] ?? 'cached_method';

if ($action === 'reset') {
	foreach ($kinds as $logKind) {
		@unlink(readonly_publish_log_file($logKind));
	}
	OPcache\volatile_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

$logFile = readonly_publish_log_file($kind);
if ($action === 'count') {
	echo $kind, '_count=', is_file($logFile) ? count(file($logFile, FILE_IGNORE_NEW_LINES)) : 0, "\n";
	return;
}

$value = match ($kind) {
	'cached_method' => CachedReadOnlyMethodState::value($logFile),
	'cached_property' => CachedReadOnlyPropertyState::value($logFile),
	'pinned_method' => PinnedReadOnlyMethodState::value($logFile),
	'pinned_property' => PinnedReadOnlyPropertyState::value($logFile),
	default => throw new RuntimeException('Unknown kind: ' . $kind),
};

echo $kind, '=', $value, "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

foreach (['cached_method', 'cached_property', 'pinned_method', 'pinned_property'] as $kind) {
	@unlink(__DIR__ . '/pinned_static_readonly_publish_001_' . $kind . '.log');
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_readonly_publish_001.php';
echo file_get_contents($base . '?action=reset');
foreach (['cached_method', 'cached_property', 'pinned_method', 'pinned_property'] as $kind) {
	echo file_get_contents($base . '?action=value&kind=' . $kind);
	echo file_get_contents($base . '?action=count&kind=' . $kind);
	echo file_get_contents($base . '?action=value&kind=' . $kind);
	echo file_get_contents($base . '?action=count&kind=' . $kind);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_readonly_publish_001.php');
foreach (['cached_method', 'cached_property', 'pinned_method', 'pinned_property'] as $kind) {
	@unlink(__DIR__ . '/pinned_static_readonly_publish_001_' . $kind . '.log');
}
?>
--EXPECT--
reset
cached_method=11
cached_method_count=1
cached_method=11
cached_method_count=1
cached_property=17
cached_property_count=1
cached_property=17
cached_property_count=1
pinned_method=13
pinned_method_count=1
pinned_method=13
pinned_method_count=1
pinned_property=19
pinned_property_count=1
pinned_property=19
pinned_property_count=1
