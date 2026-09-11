--TEST--
Packed numeric sorting preserves the order of distinct integers that compare as equal doubles
--SKIPIF--
<?php if (PHP_INT_SIZE < 8) die('skip requires 64-bit integers'); ?>
--FILE--
<?php
foreach ([2, 16, 17, 32, 64, 1025] as $size) {
    $values = [];
    for ($i = 0; $i < $size; $i++) {
        $values[] = $i % 2 ? 9007199254740992 : 9007199254740993;
    }
    foreach (['sort' => 'asort', 'rsort' => 'arsort'] as $sort => $referenceSort) {
        $actual = $values;
        $sort($actual, SORT_NUMERIC);
        if ($actual !== $values) throw new Exception('Equal numeric values reordered');

        $limits = [...$values, PHP_INT_MAX, PHP_INT_MAX - 1, PHP_INT_MIN, PHP_INT_MIN + 1];
        foreach ([SORT_NUMERIC, SORT_REGULAR] as $flags) {
            $expected = $actual = $limits;
            $referenceSort($expected, $flags);
            $sort($actual, $flags);
            if ($actual !== array_values($expected)) throw new Exception('Integer limits reordered');
        }
    }
    echo "size $size: OK\n";
}
?>
--EXPECT--
size 2: OK
size 16: OK
size 17: OK
size 32: OK
size 64: OK
size 1025: OK
