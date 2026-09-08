--TEST--
Packed sorting preserves stable comparisons for references and non-integers
--FILE--
<?php
function check($actual, $expected) {
    if ($actual !== $expected) {
        throw new Exception(var_export([$actual, $expected], true));
    }
}

foreach (['sort', 'rsort'] as $sort) {
    $left = $right = 1;
    $values = [&$left, 2, &$right];
    $sort($values);
    $left = 11;
    $right = 12;
    check($values, $sort === 'sort' ? [11, 12, 2] : [2, 11, 12]);

    $values = [1.0, 1, 0.5];
    $sort($values);
    check($values, $sort === 'sort' ? [0.5, 1.0, 1] : [1.0, 1, 0.5]);

    $values = ['1', '01', '2', '02'];
    $sort($values);
    check($values, $sort === 'sort' ? ['1', '01', '2', '02'] : ['2', '02', '1', '01']);

    $values = range(4, 1);
    $values[] = 0.5;
    $sort($values);
    check($values, $sort === 'sort' ? [0.5, ...range(1, 4)] : [...range(4, 1), 0.5]);

    $values = [3, 9, 1, 2];
    unset($values[1]);
    $sort($values);
    check($values, $sort === 'sort' ? [1, 2, 3] : [3, 2, 1]);
    echo "$sort: OK\n";
}
?>
--EXPECT--
sort: OK
rsort: OK
