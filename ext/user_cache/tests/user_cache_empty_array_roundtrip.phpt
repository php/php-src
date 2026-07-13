--TEST--
UserCache\Cache: empty arrays round-trip from the lazy (unallocated) table
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('empty-arrays');

class UserCacheEmptyArrayNode
{
	public ?UserCacheEmptyArrayNode $parent = null;
	public array $children = [];

	public function __construct(public string $name)
	{
	}

	public function __sleep(): array
	{
		return ['parent', 'children', 'name'];
	}

	public function __wakeup(): void
	{
	}
}

var_dump($cache->store('root', []));
var_dump($cache->fetch('root'));

var_dump($cache->store('nested', ['a' => [], 'b' => [[], []], 'c' => ['x' => 1]]));
var_dump($cache->fetch('nested'));

$shared = [];
$wrap = ['first' => &$shared, 'second' => &$shared];
var_dump($cache->store('shared', $wrap));
$fetched = $cache->fetch('shared');
$fetched['first'][] = 'linked';
var_dump($fetched['second']);

$root = new UserCacheEmptyArrayNode('root');
$leaf = new UserCacheEmptyArrayNode('leaf');
$leaf->parent = $root;
$root->children[] = $leaf;
var_dump($cache->store('graph', $root));
$graph = $cache->fetch('graph');
var_dump($graph->children[0]->children);
var_dump($graph->children[0]->parent === $graph);

$empty = $cache->fetch('root');
$empty[] = 42;
var_dump($empty);

unset($shared, $wrap, $fetched, $root, $leaf, $graph, $empty);
gc_collect_cycles();
?>
--EXPECT--
bool(true)
array(0) {
}
bool(true)
array(3) {
  ["a"]=>
  array(0) {
  }
  ["b"]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    array(0) {
    }
  }
  ["c"]=>
  array(1) {
    ["x"]=>
    int(1)
  }
}
bool(true)
array(1) {
  [0]=>
  string(6) "linked"
}
bool(true)
array(0) {
}
bool(true)
array(1) {
  [0]=>
  int(42)
}
