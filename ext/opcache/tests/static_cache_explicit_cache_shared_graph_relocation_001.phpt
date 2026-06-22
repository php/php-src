--TEST--
OPcache explicit volatile cache relocates unreferenced shared graph blocks during compaction
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
--FILE--
<?php

function build_relocatable_graph(): array
{
    $rows = [];
    for ($i = 0; $i < 3500; $i++) {
        $rows[] = [
            $i,
            str_repeat(chr(65 + ($i % 26)), 48),
            [$i * 2, $i * 3],
        ];
    }

    return [
        'rows' => $rows,
        'meta' => ['kind' => 'relocatable'],
    ];
}

opcache_static_cache_volatile_reset();

var_dump(OPcache\VolatileCache::getInstance('default')->store('shared_graph_relocation_first', str_repeat('A', 1500000)));
var_dump(OPcache\VolatileCache::getInstance('default')->store('shared_graph_relocation_graph', build_relocatable_graph()));
var_dump(OPcache\VolatileCache::getInstance('default')->store('shared_graph_relocation_third', str_repeat('C', 1500000)));

OPcache\VolatileCache::getInstance('default')->delete('shared_graph_relocation_first');

var_dump(OPcache\VolatileCache::getInstance('default')->store('shared_graph_relocation_merged', str_repeat('M', 3200000)));

$graph = OPcache\VolatileCache::getInstance('default')->fetch('shared_graph_relocation_graph');
var_dump($graph['rows'][123][1]);
var_dump($graph['rows'][123][2][1]);
var_dump($graph['meta']['kind']);
var_dump(strlen(OPcache\VolatileCache::getInstance('default')->fetch('shared_graph_relocation_merged')));
var_dump(strlen(OPcache\VolatileCache::getInstance('default')->fetch('shared_graph_relocation_third')));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
string(48) "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"
int(369)
string(11) "relocatable"
int(3200000)
int(1500000)
