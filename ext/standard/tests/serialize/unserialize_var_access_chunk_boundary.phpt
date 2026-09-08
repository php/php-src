--TEST--
unserialize(): R:/r: back-references resolve correctly across the internal 1018-slot var_access() chunk boundary
--FILE--
<?php

// VAR_ENTRIES_MAX (var_unserializer.c) is 1018 slots per internal chunk.
// Use enough elements that some values are only first-seen in the *second*
// chunk, so their R:/r: back-reference ids are > VAR_ENTRIES_MAX and must be
// resolved through the chunk_list jump table rather than chunk 0 directly.
$n = 1100;

$refs = [];
for ($i = 0; $i < $n; $i++) {
    $refs[$i] = $i;
}

$arr = [];
foreach ($refs as $i => &$v) {
    $arr[$i] = &$v;
}
unset($v);

// Duplicate references whose *first* occurrence is at id 0 (chunk 0), id
// 1018 (first slot of the second chunk) and id 1019 (second slot), so
// serialize() emits R:<id>; tokens that must resolve via both code paths.
$arr[$n]     = &$refs[0];
$arr[$n + 1] = &$refs[1018];
$arr[$n + 2] = &$refs[1019];

$result = unserialize(serialize($arr));

var_dump(count($result) === $n + 3);

// Mutating through one alias must be visible through the other -- this only
// holds if unserialize() reconnected the R: reference to the correct,
// still-live prior value rather than returning NULL/wrong data.
$result[0] = 'mutated-0';
$result[1018] = 'mutated-1018';
$result[1019] = 'mutated-1019';
var_dump($result[$n] === 'mutated-0');
var_dump($result[$n + 1] === 'mutated-1018');
var_dump($result[$n + 2] === 'mutated-1019');

// Same coverage for shared OBJECT identity via lowercase r:, a separate
// parser code path that also calls var_access().
class Leaf {
    public $v;
    public function __construct($v) { $this->v = $v; }
}
$objs = [];
for ($i = 0; $i < $n; $i++) {
    $objs[$i] = new Leaf($i);
}
$objs[$n] = $objs[1050]; // same object instance -> serialize() emits r:<id>;
                          // with id = 1050, in the second chunk

$out2 = unserialize(serialize($objs));
var_dump(count($out2) === $n + 1);
var_dump($out2[$n] === $out2[1050]); // same instance, not just an equal value

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
