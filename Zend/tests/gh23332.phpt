--TEST--
GH-23332 (Saturation of the HashTable iterator counter leads to UAF)
--FILE--
<?php
function gen(array &$a) {
    foreach ($a as &$v) {
        yield;
    }
}

$a = ['first' => 1, 'second' => 2, 'third' => 3];

// Saturate $a's iterator counter (it caps at 255).
$gens = [];
for ($i = 0; $i < 255; $i++) {
    $g = gen($a);
    $g->current();
    $gens[] = $g;
}
unset($g);

$array = $a;

$pass = 0;
$retained = null;
foreach ($array as $key => &$value) {
    echo "pass ", ++$pass, ": $key => $value\n";
    if ($pass === 1) {
        $retained = $array;
        $array = ['replacement' => 4242];
        continue;
    }
    for ($i = 0; $i < 254; $i++) {
        unset($gens[$i]);
    }
    $retained = null;
    unset($gens[254]);
}

echo "done\n";
?>
--EXPECT--
pass 1: first => 1
pass 2: replacement => 4242
done
