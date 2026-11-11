--TEST--
sort() and rsort() on packed arrays of integers match usort() for random input
--FILE--
<?php
$rng = new Random\Randomizer(new Random\Engine\Mt19937(20230607));
$shapes = [
    'random'   => fn ($n, $i) => $rng->getInt(PHP_INT_MIN, PHP_INT_MAX),
    'few'      => fn ($n, $i) => $rng->getInt(-3, 3),
    'sorted'   => fn ($n, $i) => $i,
    'reversed' => fn ($n, $i) => $n - $i,
    'organ'    => fn ($n, $i) => min($i, $n - 1 - $i),
];
// Around the insertion sort cut-off (16), the median-of-5 pivot (1024) and the
// stack/heap switch for the scratch buffer.
$sizes = [...range(0, 40), 63, 64, 65, 1023, 1024, 1025, 4095, 4096, 4097, 20000];

$failures = 0;
foreach ($sizes as $n) {
    foreach ($shapes as $shape => $gen) {
        $a = [];
        for ($i = 0; $i < $n; $i++) {
            $a[] = $gen($n, $i);
        }
        $inputs = ['' => $a];
        if ($n > 2) {
            unset($a[0], $a[$n >> 1]);
            $inputs[' with holes'] = $a;
        }
        foreach ($inputs as $label => $input) {
            $sorted = $input;
            sort($sorted);
            $expected = $input;
            usort($expected, fn ($x, $y) => $x <=> $y);
            if ($sorted !== $expected) {
                echo "sort() failed: $shape$label, n = $n\n";
                $failures++;
            }

            $sorted = $input;
            rsort($sorted);
            $expected = $input;
            usort($expected, fn ($x, $y) => $y <=> $x);
            if ($sorted !== $expected) {
                echo "rsort() failed: $shape$label, n = $n\n";
                $failures++;
            }
        }
    }
}
echo $failures ? "$failures failures\n" : "Done\n";
?>
--EXPECT--
Done
