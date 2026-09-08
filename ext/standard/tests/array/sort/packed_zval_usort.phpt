--TEST--
Packed usort preserves stability, input isolation, holes, exceptions and nested callbacks
--FILE--
<?php
function check($actual, $expected) {
    if ($actual !== $expected) throw new Exception(var_export([$actual, $expected], true));
}
foreach ([2, 16, 17, 32, 64, 1025] as $size) {
    $input = [];
    for ($i = 0; $i < $size; $i++) $input[] = [($size - $i) % 3, $i];
    foreach ([false, true] as $holes) {
        $values = $input;
        if ($holes) unset($values[0]);
        $original = $values;
        $expected = $values;
        uasort($expected, fn($a, $b) => $a[0] <=> $b[0]);
        check(usort($values, function ($a, $b) use (&$values, $original) {
            check($values, $original);
            return $a[0] <=> $b[0];
        }), true);
        check($values, array_values($expected));
        $values[] = 'appended';
        check(array_key_last($values), count($original));
    }

    $values = $input;
    $expected = $input;
    uasort($expected, fn($a, $b) => $a[0] <=> $b[0]);
    $first = true;
    usort($values, function ($a, $b) use (&$values, &$first) {
        if ($first) {
            $first = false;
            usort($values, fn($a, $b) => $b[1] <=> $a[1]);
        }
        return $a[0] <=> $b[0];
    });
    check($values, array_values($expected));

    $values = $input;
    try {
        usort($values, function () { throw new RuntimeException('comparison'); });
        throw new Exception('Missing exception');
    } catch (RuntimeException $e) {
        check($e->getMessage(), 'comparison');
    }
    check(array_is_list($values), true);
    $ids = array_column($values, 1);
    sort($ids);
    check($ids, range(0, $size - 1));
    check(count($input), $size);
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
