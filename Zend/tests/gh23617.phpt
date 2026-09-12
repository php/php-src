--TEST--
GH-23617: Past-the-end internal pointer picks up an append after separation and compaction
--FILE--
<?php
$makers = [
    'mixed' => function () { $a = ['a' => 1, 'b' => 2, 'c' => 3]; $a['c'] = 3; return $a; },
    'mixed with holes' => function () { $a = ['a' => 1, 'b' => 2, 'c' => 3, 'd' => 4, 'e' => 5]; unset($a['b'], $a['c']); return $a; },
    'full mixed with holes' => function () { $a = []; for ($i = 1; $i <= 8; $i++) $a["k$i"] = $i; unset($a['k2'], $a['k4'], $a['k6']); return $a; },
    'packed' => function () { $a = [1, 2, 3]; $a[] = 4; return $a; },
    'packed with holes' => function () { $a = [1, 2, 3, 4]; unset($a[1]); return $a; },
];
foreach ($makers as $name => $make) {
    foreach ([false, true] as $separate) {
        $a = $make();
        end($a);
        next($a);
        if ($separate) {
            $copy = $a;
        }
        $a[] = 'new';
        echo $name, $separate ? ' (separated): ' : ': ';
        var_dump(current($a));
        unset($copy);
    }
}
?>
--EXPECT--
mixed: string(3) "new"
mixed (separated): string(3) "new"
mixed with holes: string(3) "new"
mixed with holes (separated): string(3) "new"
full mixed with holes: string(3) "new"
full mixed with holes (separated): string(3) "new"
packed: string(3) "new"
packed (separated): string(3) "new"
packed with holes: string(3) "new"
packed with holes (separated): string(3) "new"
