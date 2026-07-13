--TEST--
UserCache\Cache: identity, cycles, and graph nesting of __sleep() objects
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('serdes-identity');

class SerdesNode
{
    public ?SerdesNode $link = null;
    public array $items = [];

    public function __sleep(): array
    {
        return ['link', 'items'];
    }

    public function __wakeup(): void
    {
    }
}

$self = new SerdesNode();
$self->link = $self;
var_dump($cache->store('self-cycle', $self));
$fetched = $cache->fetch('self-cycle');
var_dump($fetched->link === $fetched);

$shared = ['payload' => range(1, 4)];
$a = new SerdesNode();
$b = new SerdesNode();
$a->items = $shared;
$b->items = $shared;
$a->link = $b;
var_dump($cache->store('pair', $a));
$pair = $cache->fetch('pair');
var_dump($pair->link instanceof SerdesNode);
var_dump($pair->items === $pair->link->items);

$node = new SerdesNode();
$node->items = ['x' => 1];
var_dump($cache->store('nested', ['first' => $node, 'second' => $node, 'plain' => [1, 2, 3]]));
$nested = $cache->fetch('nested');
var_dump($nested['first'] instanceof SerdesNode);
var_dump($nested['first'] === $nested['second']);
var_dump($nested['first']->items['x']);
var_dump($nested['plain']);

unset($self, $fetched, $shared, $a, $b, $pair, $node, $nested);
gc_collect_cycles();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
