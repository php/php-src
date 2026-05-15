--TEST--
OPcache PersistentStatic skips nested array local-copy mutation publish
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/persistent_static_local_copy_array_mutation_publish_001.php', <<<'PHP'
<?php
class PersistentStaticLocalCopyPublishProbe
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

class PersistentStaticLocalCopyArrayState
{
	#[OPcache\PersistentStatic]
	public static array $value = [];
}

function local_copy_log_file(): string
{
	return __DIR__ . '/persistent_static_local_copy_array_mutation_publish_001.log';
}

function local_copy_log_count(): int
{
	$logFile = local_copy_log_file();
	return is_file($logFile) ? count(file($logFile, FILE_IGNORE_NEW_LINES)) : 0;
}

$action = $_GET['action'] ?? 'read';
$logFile = local_copy_log_file();

if ($action === 'reset') {
	@unlink($logFile);
	OPcache\persistent_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	PersistentStaticLocalCopyArrayState::$value = [
		'nested' => ['seed'],
		'probe' => new PersistentStaticLocalCopyPublishProbe($logFile, 42),
	];
	echo 'seed-static=', count(PersistentStaticLocalCopyArrayState::$value['nested']), "\n";
	echo 'seed-log=', local_copy_log_count(), "\n";
	return;
}

if ($action === 'local') {
	$copy = PersistentStaticLocalCopyArrayState::$value['nested'];
	$copy[] = 'local';
	echo 'local-copy=', count($copy), "\n";
	echo 'local-static=', count(PersistentStaticLocalCopyArrayState::$value['nested']), "\n";
	echo 'local-log=', local_copy_log_count(), "\n";
	return;
}

if ($action === 'direct') {
	PersistentStaticLocalCopyArrayState::$value['nested'][] = 'direct';
	echo 'direct-static=', count(PersistentStaticLocalCopyArrayState::$value['nested']), "\n";
	echo 'direct-log=', local_copy_log_count(), "\n";
	return;
}

echo 'read-static=', count(PersistentStaticLocalCopyArrayState::$value['nested']), "\n";
echo 'read-log=', local_copy_log_count(), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

@unlink(__DIR__ . '/persistent_static_local_copy_array_mutation_publish_001.log');

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.persistent_size_mb=32 -d opcache.optimization_level=0 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_local_copy_array_mutation_publish_001.php';
echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=local');
echo file_get_contents($base . '?action=read');
echo file_get_contents($base . '?action=direct');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/persistent_static_local_copy_array_mutation_publish_001.php');
@unlink(__DIR__ . '/persistent_static_local_copy_array_mutation_publish_001.log');
?>
--EXPECT--
reset
seed-static=1
seed-log=1
local-copy=2
local-static=1
local-log=1
read-static=1
read-log=1
direct-static=2
direct-log=2
read-static=2
read-log=2
