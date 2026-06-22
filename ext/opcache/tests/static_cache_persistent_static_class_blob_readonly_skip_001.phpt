--TEST--
OPcache StableStatic skips class blob publish for read-only cached requests
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/stable_static_009.php', <<<'PHP'
<?php
class PublishProbe
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

#[OPcache\StableStatic]
class ReadOnlyBlobState
{
	public static function value(string $logFile): int
	{
		static $probe = null;

		if ($probe === null) {
			$probe = new PublishProbe($logFile, 7);
		}

		return $probe->value;
	}
}

$logFile = __DIR__ . '/stable_static_009.log';
$action = $_GET['action'] ?? 'value';

if ($action === 'reset') {
	@unlink($logFile);
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'count') {
	echo is_file($logFile) ? count(file($logFile, FILE_IGNORE_NEW_LINES)) : 0, "\n";
	return;
}

echo ReadOnlyBlobState::value($logFile), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

@unlink(__DIR__ . '/stable_static_009.log');

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.stable_size_mb=32 -d opcache.file_update_protection=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_009.php?action=reset');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_009.php?action=value');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_009.php?action=count');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_009.php?action=value');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_009.php?action=count');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_static_009.php');
@unlink(__DIR__ . '/stable_static_009.log');
?>
--EXPECT--
reset
7
1
7
1
