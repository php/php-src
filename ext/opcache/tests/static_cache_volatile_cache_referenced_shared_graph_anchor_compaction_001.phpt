--TEST--
OPcache volatile cache keeps referenced shared graph blocks anchored during compaction
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
--FILE--
<?php

function build_anchored_graph(): array
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
        'meta' => ['kind' => 'anchored'],
    ];
}

OPcache\VolatileCache::clear();

var_dump(OPcache\VolatileCache::set('shared_graph_anchor_first', str_repeat('A', 1500000)));
var_dump(OPcache\VolatileCache::set('shared_graph_anchor_graph', build_anchored_graph()));
var_dump(OPcache\VolatileCache::set('shared_graph_anchor_third', str_repeat('C', 1500000)));

$graph = OPcache\VolatileCache::get('shared_graph_anchor_graph');
OPcache\VolatileCache::delete('shared_graph_anchor_first');

var_dump(OPcache\VolatileCache::set('shared_graph_anchor_merged', str_repeat('M', 3200000)));
var_dump($graph['rows'][123][1]);
var_dump($graph['rows'][123][2][1]);
var_dump($graph['meta']['kind']);
var_dump(OPcache\VolatileCache::get('shared_graph_anchor_merged', 'MISS'));
var_dump(strlen(OPcache\VolatileCache::get('shared_graph_anchor_third')));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
string(48) "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"
int(369)
string(8) "anchored"
string(4) "MISS"
int(1500000)
