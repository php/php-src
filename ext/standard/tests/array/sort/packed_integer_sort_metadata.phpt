--TEST--
Packed integer sorting resets array metadata and preserves copy on write
--FILE--
<?php
function check($actual, $expected) {
    if ($actual !== $expected) {
        throw new Exception(var_export([$actual, $expected], true));
    }
}

foreach (['sort', 'rsort'] as $sort) {
    $values = [3, 1, 2, 9];
    unset($values[3]);
    end($values);
    $sort($values);
    $expected = $sort === 'sort' ? [1, 2, 3] : [3, 2, 1];
    check($values, $expected);
    check(key($values), 0);
    check(current($values), $expected[0]);
    $values[] = 99;
    check($values, [...$expected, 99]);

    $values = [3, 1, 2];
    unset($values[2]);
    $values[] = 2;
    $sort($values);
    $values[] = 99;
    check($values, [...$expected, 99]);

    $original = [3, 1, 2];
    $values = $original;
    $alias = &$values;
    $sort($alias);
    check($values, $expected);
    check($original, [3, 1, 2]);
    unset($alias);

    $values = range(65, 1);
    unset($values[64]);
    $original = $values;
    end($values);
    $sort($values);
    $expected = $sort === 'sort' ? range(2, 65) : range(65, 2);
    check($values, $expected);
    check($original, range(65, 2));
    check(key($values), 0);
    check(current($values), $expected[0]);
    $values[] = 99;
    check($values, [...$expected, 99]);

    $values = [11, 22];
    unset($values[1], $values[0]);
    $sort($values);
    $values[] = 123;
    check($values, [2 => 123]);

    $values = [11, 22];
    unset($values[0]);
    $sort($values);
    $values[] = 123;
    check($values, [22, 123]);
    echo "$sort: OK\n";
}
?>
--EXPECT--
sort: OK
rsort: OK
