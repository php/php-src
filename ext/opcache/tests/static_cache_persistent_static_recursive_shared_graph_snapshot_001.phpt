--TEST--
OPcache PersistentStatic snapshots recursive shared-graph state across requests
--EXTENSIONS--
opcache
spl
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/persistent_static_011.php', <<<'PHP'
<?php
class GraphChild
{
	public function __construct(public int $count) {}
}

class GraphNode
{
	public function __construct(
		public GraphChild $child,
		public array $trail,
	) {}
}

class RecursivePropertyState
{
	#[OPcache\PersistentStatic]
	public static array $state = [];
}

if (RecursivePropertyState::$state === []) {
	RecursivePropertyState::$state = [
		'graph' => new GraphNode(new GraphChild(1), ['hits' => []]),
		'serial' => new ArrayObject([
			'box' => new GraphChild(10),
			'nested' => ['hits' => []],
		]),
	];
}

$state = &RecursivePropertyState::$state;
$state['graph']->child->count++;
$state['graph']->trail['hits'][] = $state['graph']->child->count;

$serial = $state['serial'];
$serial['box']->count++;
$nested = $serial['nested'];
$nested['hits'][] = $serial['box']->count;
$serial['nested'] = $nested;

echo $state['graph']->child->count, ',', count($state['graph']->trail['hits']), ',', $serial['box']->count, ',', count($serial['nested']['hits']), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.persistent_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_011.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_011.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_011.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/persistent_static_011.php');
?>
--EXPECT--
2,1,11,1
2,1,11,1
2,1,11,1
