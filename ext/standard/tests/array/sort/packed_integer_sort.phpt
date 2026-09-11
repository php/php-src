--TEST--
Packed integer sort and rsort at insertion sort and pivot selection boundaries
--FILE--
<?php
function check($actual, $expected) {
    if ($actual !== $expected) {
        throw new Exception(var_export([$actual, $expected], true));
    }
}

foreach ([2, 16, 17, 32, 63, 64, 65, 1024, 1025] as $size) {
    $ascending = range(1, $size);
    $descending = array_reverse($ascending);
    $permuted = array_merge(array_slice($ascending, 1), [1]);
    $duplicates = array_fill(0, $size, 7);
    $limits = array_fill(0, $size - 2, 0);
    $limits[] = PHP_INT_MAX;
    $limits[] = PHP_INT_MIN;
    $sortedLimits = array_merge([PHP_INT_MIN], array_fill(0, $size - 2, 0), [PHP_INT_MAX]);
    foreach ([[$ascending, $ascending], [$descending, $ascending],
              [$permuted, $ascending], [$duplicates, $duplicates],
              [$limits, $sortedLimits]] as [$input, $expected]) {
        // Unknown flags also resolve to regular comparison.
        foreach ([SORT_REGULAR, SORT_REGULAR | SORT_FLAG_CASE, 12345] as $flags) {
            $values = $input;
            check(sort($values, $flags), true);
            check($values, $expected);
            $values = $input;
            check(rsort($values, $flags), true);
            check($values, array_reverse($expected));
        }
    }
    echo "size $size: OK\n";
}

// Other modes and sorts that preserve keys must keep their own comparators.
$values = [10, 2, 1];
sort($values, SORT_STRING);
check($values, [1, 10, 2]);
rsort($values, SORT_STRING);
check($values, [2, 10, 1]);
sort($values, SORT_NUMERIC);
check($values, [1, 2, 10]);
$values = [3, 1, 2];
asort($values);
check($values, [1 => 1, 2 => 2, 0 => 3]);
arsort($values);
check($values, [0 => 3, 2 => 2, 1 => 1]);
ksort($values);
check($values, [3, 1, 2]);
krsort($values);
check($values, [2 => 2, 1 => 1, 0 => 3]);
echo "other comparators: OK\n";
?>
--EXPECT--
size 2: OK
size 16: OK
size 17: OK
size 32: OK
size 63: OK
size 64: OK
size 65: OK
size 1024: OK
size 1025: OK
other comparators: OK
