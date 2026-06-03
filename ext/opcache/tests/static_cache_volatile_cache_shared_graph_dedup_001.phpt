--TEST--
OPcache volatile cache decodes large homogeneous object graphs (string dedup + class memo) losslessly and independently
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=64
--FILE--
<?php

class Node
{
    public string $kind = '';
    public string $label = '';
    public array $tags = [];
    public ?Node $next = null;
}

/* A homogeneous list of many objects shares one class name and the same
 * property names, and repeats string values, exercising the build-time string
 * dedup and the decode-time class memo. */
function make_list(int $n): array
{
    $list = [];
    for ($i = 0; $i < $n; $i++) {
        $node = new Node();
        $node->kind = 'repeated-kind';
        $node->label = 'repeated-label';
        $node->tags = ['alpha', 'beta', 'alpha', 'beta'];
        $list[] = $node;
    }

    return $list;
}

$payload = [
    'list' => make_list(200),
    'meta' => ['kind' => 'repeated-kind', 'again' => 'repeated-kind'],
];

var_dump(OPcache\VolatileCache::getInstance('default')->store('graph', $payload));

$a = OPcache\VolatileCache::getInstance('default')->fetch('graph');

/* Lossless round trip despite heavy string duplication. */
var_dump(serialize($a) === serialize($payload));
var_dump(count($a['list']));
var_dump($a['list'][0] instanceof Node);
var_dump($a['list'][199]->kind);
var_dump(implode(',', $a['list'][199]->tags));
var_dump($a['meta']['again']);

/* Only strings are deduplicated; distinct objects stay distinct. */
var_dump(spl_object_id($a['list'][0]) !== spl_object_id($a['list'][1]));

/* Each fetch decodes an independent copy, so mutating one must not affect
 * another fetch or the stored value. */
$b = OPcache\VolatileCache::getInstance('default')->fetch('graph');
var_dump(spl_object_id($a['list'][0]) !== spl_object_id($b['list'][0]));
$b['list'][0]->kind = 'mutated';
$c = OPcache\VolatileCache::getInstance('default')->fetch('graph');
var_dump($c['list'][0]->kind);

/* A deeply nested homogeneous chain exercises the class memo across the
 * recursive decode. */
function make_chain(int $depth): Node
{
    $head = new Node();
    $cur = $head;
    for ($i = 0; $i < $depth; $i++) {
        $cur->kind = 'chain';
        $cur->label = 'n' . $i;
        $cur->next = new Node();
        $cur = $cur->next;
    }

    return $head;
}

var_dump(OPcache\VolatileCache::getInstance('default')->store('chain', make_chain(500)));

$chain = OPcache\VolatileCache::getInstance('default')->fetch('chain');
$len = 0;
for ($n = $chain; $n !== null; $n = $n->next) {
    $len++;
}
var_dump($len);

?>
--EXPECT--
bool(true)
bool(true)
int(200)
bool(true)
string(13) "repeated-kind"
string(21) "alpha,beta,alpha,beta"
string(13) "repeated-kind"
bool(true)
bool(true)
string(13) "repeated-kind"
bool(true)
int(501)
