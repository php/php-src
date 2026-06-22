--TEST--
OPcache volatile cache: relocating a shared graph rebases direct (zero-copy) arrays nested under a reference or a __serialize state, not only under plain arrays/objects
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
use OPcache\VolatileCache as VC;

class K_Hook
{
    public array $data = [];

    public function __serialize(): array
    {
        return ['data' => $this->data];
    }

    public function __unserialize(array $d): void
    {
        $this->data = $d['data'];
    }
}

function build_graph(): array
{
    $rows = [];
    for ($i = 0; $i < 2000; $i++) {
        $rows[] = [$i, str_repeat(chr(65 + ($i % 26)), 48)];
    }

    /* A direct (immutable string) array reachable only through a reference. */
    $reftarget = ['rs1' => str_repeat('R', 41), 'rs2' => str_repeat('S', 42)];

    /* A direct string array embedded in a __serialize state. */
    $h = new K_Hook();
    $h->data = ['hs1' => str_repeat('H', 43)];

    $g = ['rows' => $rows, 'hook' => $h];
    $g['a'] = &$reftarget;
    $g['b'] = &$reftarget;

    return $g;
}

VC::getInstance('default')->clear();

var_dump(VC::getInstance('default')->store('reloc_first', str_repeat('A', 1500000)));
var_dump(VC::getInstance('default')->store('reloc_graph', build_graph()));
var_dump(VC::getInstance('default')->store('reloc_third', str_repeat('C', 1500000)));

VC::getInstance('default')->delete('reloc_first');

/* Forces compaction; the unreferenced graph block is moved and must be rebased. */
var_dump(VC::getInstance('default')->store('reloc_merged', str_repeat('M', 3200000)));

$g = VC::getInstance('default')->fetch('reloc_graph');
echo $g['a']['rs1'], "\n";
echo $g['a']['rs2'], "\n";
var_dump($g['a'] === $g['b']);
echo $g['hook']->data['hs1'], "\n";
echo $g['rows'][123][1], "\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
bool(true)
HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
