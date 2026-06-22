--TEST--
OPcache StableCache::clear drops combined stable entries without corrupting refill or volatile entries
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/stable_cache_clear_combined_001.php', <<<'PHP'
<?php

class StableClearCombinedNode
{
	public function __construct(public array $rows) {}
}

function build_rows(string $prefix, int $multiplier): array
{
	$rows = [];

	for ($i = 0; $i < 1024; $i++) {
		$rows[] = [
			'id' => $i,
			'text' => str_repeat($prefix, 32),
			'nested' => ['value' => $i * $multiplier],
		];
	}

	return $rows;
}

function build_graph(string $prefix, int $multiplier): StableClearCombinedNode
{
	return new StableClearCombinedNode(build_rows($prefix, $multiplier));
}

$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
	opcache_static_cache_volatile_reset();
	OPcache\StableCache::getInstance('default')->clear();
	var_dump(OPcache\VolatileCache::getInstance('default')->store('local-string', str_repeat('L', 400000)));
	OPcache\StableCache::getInstance('default')->store('stable-string', str_repeat('G', 400000));
	OPcache\StableCache::getInstance('default')->store('stable-graph', build_graph('O', 3));
	$graph = OPcache\StableCache::getInstance('default')->fetch('stable-graph');
	var_dump(strlen(OPcache\StableCache::getInstance('default')->fetch('stable-string')));
	var_dump($graph->rows[123]['text']);
	return;
}

if ($action === 'clear') {
	OPcache\StableCache::getInstance('default')->clear();
	var_dump(strlen(OPcache\VolatileCache::getInstance('default')->fetch('local-string', 'missing-volatile')));
	var_dump(OPcache\StableCache::getInstance('default')->fetch('stable-string', 'missing-stable'));
	var_dump(OPcache\StableCache::getInstance('default')->fetch('stable-graph', 'missing-graph'));
	return;
}

if ($action === 'refill') {
	OPcache\StableCache::getInstance('default')->store('stable-string', str_repeat('H', 400000));
	OPcache\StableCache::getInstance('default')->store('stable-graph', build_graph('N', 7));
	$graph = OPcache\StableCache::getInstance('default')->fetch('stable-graph');
	var_dump(strlen(OPcache\StableCache::getInstance('default')->fetch('stable-string')));
	var_dump($graph->rows[123]['text']);
	var_dump($graph->rows[123]['nested']['value']);
	return;
}

$graph = OPcache\StableCache::getInstance('default')->fetch('stable-graph');
var_dump(strlen(OPcache\VolatileCache::getInstance('default')->fetch('local-string', 'missing-volatile')));
var_dump(strlen(OPcache\StableCache::getInstance('default')->fetch('stable-string')));
var_dump($graph->rows[123]['text']);
var_dump($graph->rows[123]['nested']['value']);
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=8 -d opcache.static_cache.stable_size_mb=8 -d opcache.file_update_protection=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/stable_cache_clear_combined_001.php';
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=clear');
echo file_get_contents($base . '?action=refill');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_cache_clear_combined_001.php');
?>
--EXPECT--
bool(true)
int(400000)
string(32) "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"
int(400000)
string(14) "missing-stable"
string(13) "missing-graph"
int(400000)
string(32) "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"
int(861)
int(400000)
int(400000)
string(32) "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"
int(861)
