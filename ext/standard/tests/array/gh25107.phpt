--TEST--
GH-25107 (array_unshift() skips values during reference iteration)
--FILE--
<?php
function single(bool $mixed, string $mode): void {
    $a = $mixed
        ? ['a' => 'a', 'b' => 'b', 'c' => 'c', 'd' => 'd', 'e' => 'e']
        : ['a', 'b', 'c', 'd', 'e'];
    if ($mode !== 'dense') {
        unset($a[$mixed ? 'a' : 0]);
    }
    $seen = [];
    foreach ($a as &$v) {
        $seen[] = $v;
        if ($mode === 'end' && $v === 'e') {
            array_unshift($a, 'x');
            $a[] = 'after';
        } elseif ($mode !== 'end' && $v === 'b') {
            if ($mode === 'next-hole') {
                unset($a[$mixed ? 'c' : 2]);
            }
            if ($mode === 'zero') {
                array_unshift($a);
            } elseif ($mode === 'many') {
                array_unshift($a, 'x', 'y', 'z');
            } else {
                array_unshift($a, 'x');
            }
        }
    }
    unset($v);
    echo ($mixed ? 'mixed' : 'packed'), ' ', $mode, ' ', json_encode($seen), "\n";
}

function nested(bool $mixed, bool $samePosition): void {
    $a = $mixed
        ? ['a' => 'a', 'b' => 'b', 'c' => 'c', 'd' => 'd', 'e' => 'e', 'f' => 'f']
        : ['a', 'b', 'c', 'd', 'e', 'f'];
    unset($a[$mixed ? 'a' : 0], $a[$mixed ? 'd' : 3]);
    $outerSeen = [];
    $innerSeen = [];
    foreach ($a as &$outer) {
        $outerSeen[] = $outer;
        if ($outer !== 'b') {
            continue;
        }
        foreach ($a as &$inner) {
            $innerSeen[] = $inner;
            if ($inner === ($samePosition ? 'b' : 'c')) {
                array_unshift($a, 'x', 'y');
            }
        }
        unset($inner);
    }
    unset($outer);
    echo ($mixed ? 'mixed' : 'packed'), ' ', ($samePosition ? 'same' : 'different'),
        ' ', json_encode($outerSeen), ' ', json_encode($innerSeen), "\n";
}

foreach ([false, true] as $mixed) {
    foreach (['dense', 'one', 'many', 'zero', 'next-hole', 'end'] as $mode) {
        single($mixed, $mode);
    }
    nested($mixed, false);
    nested($mixed, true);
}
?>
--EXPECT--
packed dense ["a","b","c","d","e"]
packed one ["b","c","d","e"]
packed many ["b","c","d","e"]
packed zero ["b","c","d","e"]
packed next-hole ["b","d","e"]
packed end ["b","c","d","e","after"]
packed different ["b","c","e","f"] ["b","c","e","f"]
packed same ["b","c","e","f"] ["b","c","e","f"]
mixed dense ["a","b","c","d","e"]
mixed one ["b","c","d","e"]
mixed many ["b","c","d","e"]
mixed zero ["b","c","d","e"]
mixed next-hole ["b","d","e"]
mixed end ["b","c","d","e","after"]
mixed different ["b","c","e","f"] ["b","c","e","f"]
mixed same ["b","c","e","f"] ["b","c","e","f"]
