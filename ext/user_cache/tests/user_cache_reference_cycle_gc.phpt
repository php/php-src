--TEST--
UserCache\Cache: plain cyclic object graph round-trips and remains collectable
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('reference-cycle-gc');

class UCPlainCycleNode
{
	public ?UCPlainCycleNode $parent = null;
	public array $children = [];

	public function __construct(public string $name)
	{
	}
}

$root = new UCPlainCycleNode('root');
$leaf = new UCPlainCycleNode('leaf');
$leaf->parent = $root;
$root->children[] = $leaf;

var_dump($cache->store('graph', $root));

$graph = $cache->fetch('graph');
echo "decoded names: ", $graph->name, " ", $graph->children[0]->name, "\n";
echo "back edge identity: ";
var_dump($graph->children[0]->parent === $graph);
echo "decoded graph is a copy: ";
var_dump($graph !== $root);
echo "leaf children: ";
var_dump($graph->children[0]->children);

$again = $cache->fetch('graph');
echo "fetches independent: ";
var_dump($again !== $graph);
echo "fetched back edge identity: ";
var_dump($again->children[0]->parent === $again);

unset($root, $leaf, $graph, $again);
gc_collect_cycles();
?>
--EXPECT--
bool(true)
decoded names: root leaf
back edge identity: bool(true)
decoded graph is a copy: bool(true)
leaf children: array(0) {
}
fetches independent: bool(true)
fetched back edge identity: bool(true)
