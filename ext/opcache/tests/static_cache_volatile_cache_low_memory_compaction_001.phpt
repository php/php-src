--TEST--
OPcache volatile cache compacts before remaining tail memory drops below 3 MiB
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
--FILE--
<?php

function build_low_memory_graph(): array
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
        'meta' => ['kind' => 'low-memory'],
    ];
}

OPcache\volatile_clear();

var_dump(OPcache\volatile_store('low_memory_first', str_repeat('A', 1500000)));
var_dump(OPcache\volatile_store('low_memory_graph', build_low_memory_graph()));
var_dump(OPcache\volatile_store('low_memory_third', str_repeat('C', 1500000)));

OPcache\volatile_delete('low_memory_first');

var_dump(OPcache\volatile_store('low_memory_trigger', str_repeat('T', 1700000)));
$graph = OPcache\volatile_fetch('low_memory_graph');
OPcache\volatile_delete('low_memory_trigger');

var_dump(OPcache\volatile_store('low_memory_merged', str_repeat('M', 3200000)));
var_dump($graph['rows'][123][1]);
var_dump($graph['rows'][123][2][1]);
var_dump($graph['meta']['kind']);
var_dump(strlen(OPcache\volatile_fetch('low_memory_merged')));
var_dump(strlen(OPcache\volatile_fetch('low_memory_third')));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(48) "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"
int(369)
string(10) "low-memory"
int(3200000)
int(1500000)
