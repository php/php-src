--TEST--
OPcache persistent_clear drops combined persistent entries without corrupting refill or volatile entries
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/persistent_cache_clear_combined_001.php', <<<'PHP'
<?php

class PersistentClearCombinedNode
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

function build_graph(string $prefix, int $multiplier): PersistentClearCombinedNode
{
	return new PersistentClearCombinedNode(build_rows($prefix, $multiplier));
}

$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
	OPcache\volatile_clear();
	OPcache\persistent_clear();
	var_dump(OPcache\volatile_store('local-string', str_repeat('L', 400000)));
	OPcache\persistent_store('persistent-string', str_repeat('G', 400000));
	OPcache\persistent_store('persistent-graph', build_graph('O', 3));
	$graph = OPcache\persistent_fetch('persistent-graph');
	var_dump(strlen(OPcache\persistent_fetch('persistent-string')));
	var_dump($graph->rows[123]['text']);
	return;
}

if ($action === 'clear') {
	OPcache\persistent_clear();
	var_dump(strlen(OPcache\volatile_fetch('local-string', 'missing-volatile')));
	var_dump(OPcache\persistent_fetch('persistent-string', 'missing-persistent'));
	var_dump(OPcache\persistent_fetch('persistent-graph', 'missing-graph'));
	return;
}

if ($action === 'refill') {
	OPcache\persistent_store('persistent-string', str_repeat('H', 400000));
	OPcache\persistent_store('persistent-graph', build_graph('N', 7));
	$graph = OPcache\persistent_fetch('persistent-graph');
	var_dump(strlen(OPcache\persistent_fetch('persistent-string')));
	var_dump($graph->rows[123]['text']);
	var_dump($graph->rows[123]['nested']['value']);
	return;
}

$graph = OPcache\persistent_fetch('persistent-graph');
var_dump(strlen(OPcache\volatile_fetch('local-string', 'missing-volatile')));
var_dump(strlen(OPcache\persistent_fetch('persistent-string')));
var_dump($graph->rows[123]['text']);
var_dump($graph->rows[123]['nested']['value']);
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=8 -d opcache.static_cache.persistent_size_mb=8 -d opcache.file_update_protection=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_cache_clear_combined_001.php';
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=clear');
echo file_get_contents($base . '?action=refill');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/persistent_cache_clear_combined_001.php');
?>
--EXPECT--
bool(true)
int(400000)
string(32) "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"
int(400000)
string(18) "missing-persistent"
string(13) "missing-graph"
int(400000)
string(32) "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"
int(861)
int(400000)
int(400000)
string(32) "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"
int(861)
