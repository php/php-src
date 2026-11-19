--TEST--
Array addition with an empty operand
--FILE--
<?php

function show(string $label, array $result): void
{
    $current = key($result);
    $result[-5] = 'negative';
    $result[] = 'appended';
    echo $label, ': current=', json_encode($current), ' keys=', json_encode(array_keys($result)), PHP_EOL;
}

$empty = [];

echo '-- empty right operand', PHP_EOL;
$hash = ['x' => 1, 'y' => 2];
show('hash', $hash + $empty);
next($hash);
show('hash, moved pointer', $hash + $empty);
next($hash);
show('hash, exhausted pointer', $hash + $empty);
$list = [1, 2, 3];
unset($list[2]);
show('list, stale next index', $list + $empty);
$emptied = [1];
unset($emptied[0]);
show('emptied list', $emptied + $empty);

echo '-- empty left operand', PHP_EOL;
$hash = ['x' => 1, 'y' => 2];
show('hash', $empty + $hash);
next($hash);
show('hash, moved pointer', $empty + $hash);
$hash = ['x' => 1, 5 => 2];
show('hash, integer key', $empty + $hash);
unset($hash[5]);
show('hash, removed integer key', $empty + $hash);
$list = [1, 2, 3];
show('list', $empty + $list);
next($list);
show('list, moved pointer', $empty + $list);
unset($list[2]);
show('list, stale next index', $empty + $list);
$list = [1, 2, 3];
unset($list[0]);
show('list, leading hole', $empty + $list);
show('emptied list on the left', $emptied + ['x' => 1]);

echo '-- both empty', PHP_EOL;
$popped = [1];
array_pop($popped);
show('empty + popped', $empty + $popped);
show('popped + empty', $popped + $empty);
show('empty + emptied', $empty + $emptied);
show('emptied + empty', $emptied + $empty);

echo '-- compound assignment', PHP_EOL;
$target = [];
$target += $popped;
show('empty += popped', $target);
$target = [];
$target += ['x' => 1, 'y' => 2];
show('empty += hash', $target);
$target = ['x' => 1];
$target += $empty;
show('hash += empty', $target);

echo '-- the operands are left untouched', PHP_EOL;
const DEFAULTS = ['a' => 1, 'b' => 2];
$result = $empty + DEFAULTS;
$result['c'] = 3;
$other = DEFAULTS + $empty;
$other['c'] = 3;
$target = [];
$target += DEFAULTS;
$target['c'] = 3;
echo json_encode(DEFAULTS), ' ', json_encode($result), ' ', json_encode($other), ' ', json_encode($target), PHP_EOL;

$source = ['k' => 'v'];
$source['l'] = 'w';
$copy = $source;
$source += $empty;
$source['m'] = 'x';
$result = $empty + $copy;
$result['k'] = 'changed';
echo json_encode($copy), ' ', json_encode($source), ' ', json_encode($result), PHP_EOL;

$value = 1;
$withReference = ['r' => &$value];
$result = $empty + $withReference;
$result['r'] = 2;
echo $value, PHP_EOL;

?>
--EXPECT--
-- empty right operand
hash: current="x" keys=["x","y",-5,-4]
hash, moved pointer: current="y" keys=["x","y",-5,-4]
hash, exhausted pointer: current="x" keys=["x","y",-5,-4]
list, stale next index: current=0 keys=[0,1,-5,3]
emptied list: current=null keys=[-5,1]
-- empty left operand
hash: current="x" keys=["x","y",-5,-4]
hash, moved pointer: current="x" keys=["x","y",-5,-4]
hash, integer key: current="x" keys=["x",5,-5,6]
hash, removed integer key: current="x" keys=["x",-5,-4]
list: current=0 keys=[0,1,2,-5,3]
list, moved pointer: current=0 keys=[0,1,2,-5,3]
list, stale next index: current=0 keys=[0,1,-5,2]
list, leading hole: current=1 keys=[1,2,-5,3]
emptied list on the left: current="x" keys=["x",-5,1]
-- both empty
empty + popped: current=null keys=[-5,-4]
popped + empty: current=null keys=[-5,0]
empty + emptied: current=null keys=[-5,-4]
emptied + empty: current=null keys=[-5,1]
-- compound assignment
empty += popped: current=null keys=[-5,-4]
empty += hash: current="x" keys=["x","y",-5,-4]
hash += empty: current="x" keys=["x",-5,-4]
-- the operands are left untouched
{"a":1,"b":2} {"a":1,"b":2,"c":3} {"a":1,"b":2,"c":3} {"a":1,"b":2,"c":3}
{"k":"v","l":"w"} {"k":"v","l":"w","m":"x"} {"k":"changed","l":"w"}
2
