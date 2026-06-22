--TEST--
OPcache explicit volatile and stable caches rebase shared graph direct arrays during relocation
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
opcache.static_cache.stable_size_mb=8
--FILE--
<?php

class SharedGraphRelocationNode
{
    public function __construct(
        public string $name,
        public array $rows,
    ) {}
}

function build_shared_graph_relocation_payload(string $label, int $multiplier): array
{
    $rows = [];
    for ($i = 0; $i < 3000; $i++) {
        $rows[] = [
            'id' => $i,
            'text' => str_repeat($label, 48),
            'nested' => [$i * $multiplier, $i * $multiplier + 1],
        ];
    }

    return [
        'object' => new SharedGraphRelocationNode('node-' . $label, $rows),
        'plain' => [
            'label' => $label,
            'tail' => str_repeat($label, 256),
        ],
    ];
}

function cache_instance_for_relocation(string $backend): OPcache\StaticCacheInterface
{
    return $backend === 'volatile'
        ? OPcache\VolatileCache::getInstance('default')
        : OPcache\StableCache::getInstance('default');
}

function cache_clear_for_relocation(string $backend): void
{
    if ($backend === 'volatile') {
        opcache_static_cache_volatile_reset();
    } else {
        cache_instance_for_relocation($backend)->clear();
    }
}

function cache_store_for_relocation(string $backend, string $key, mixed $value): bool
{
    return cache_instance_for_relocation($backend)->store($key, $value);
}

function cache_fetch_for_relocation(string $backend, string $key): mixed
{
    return cache_instance_for_relocation($backend)->fetch($key);
}

function cache_delete_for_relocation(string $backend, string $key): void
{
    cache_instance_for_relocation($backend)->delete($key);
}

function run_shared_graph_relocation(string $backend, string $label): void
{
    echo "-- {$backend} --\n";

    cache_clear_for_relocation($backend);

    $prefix = $backend . '_shared_graph_relocation_';
    var_dump(cache_store_for_relocation($backend, $prefix . 'first', str_repeat('A', 1200000)));
    var_dump(cache_store_for_relocation($backend, $prefix . 'graph', build_shared_graph_relocation_payload($label, 5)));
    var_dump(cache_store_for_relocation($backend, $prefix . 'third', str_repeat('C', 1200000)));

    cache_delete_for_relocation($backend, $prefix . 'first');

    var_dump(cache_store_for_relocation($backend, $prefix . 'merged', str_repeat('M', 2400000)));

    $graph = cache_fetch_for_relocation($backend, $prefix . 'graph');
    var_dump($graph['object'] instanceof SharedGraphRelocationNode);
    var_dump($graph['object']->name);
    var_dump($graph['object']->rows[123]['text']);
    var_dump($graph['object']->rows[123]['nested'][1]);
    var_dump($graph['plain']['label']);
    var_dump(strlen($graph['plain']['tail']));
    var_dump(strlen(cache_fetch_for_relocation($backend, $prefix . 'merged')));
    var_dump(strlen(cache_fetch_for_relocation($backend, $prefix . 'third')));
}

run_shared_graph_relocation('volatile', 'V');
run_shared_graph_relocation('stable', 'P');

?>
--EXPECT--
-- volatile --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(6) "node-V"
string(48) "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
int(616)
string(1) "V"
int(256)
int(2400000)
int(1200000)
-- stable --
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(6) "node-P"
string(48) "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP"
int(616)
string(1) "P"
int(256)
int(2400000)
int(1200000)
