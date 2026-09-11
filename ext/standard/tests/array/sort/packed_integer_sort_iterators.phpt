--TEST--
Sorting packed integer arrays during by-reference foreach preserves iterator positions
--FILE--
<?php
function check($actual, $expected) {
    if ($actual !== $expected) {
        throw new Exception(var_export([$actual, $expected], true));
    }
}

foreach ([['sort', 3], ['sort', 64], ['rsort', 3], ['rsort', 64]] as [$sort, $size]) {
    foreach ([false, true] as $removeCurrent) {
        $values = [$size + 1, ...range($size, 1)];
        $visited = [];
        $first = true;
        foreach ($values as $key => &$value) {
            $visited[] = [$key, $value];
            if ($first) {
                $first = false;
                if ($removeCurrent) {
                    // Remove the referenced element while keeping the iterator active.
                    array_shift($values);
                    foreach (array_keys($values) as $index) {
                        check(ReflectionReference::fromArrayElement($values, $index), null);
                    }
                }
                $sort($values);
            }
        }
        unset($value);
        $count = $size + (int) !$removeCurrent;
        $expected = $sort === 'sort' ? range(1, $count) : range($count, 1);
        $expectedVisited = [[0, $size + 1]];
        for ($index = $removeCurrent ? 0 : 1; $index < $count; $index++) {
            $expectedVisited[] = [$index, $expected[$index]];
        }
        check($visited, $expectedVisited);
        check($values, $expected);
    }
    echo "$sort size $size: OK\n";
}
?>
--EXPECT--
sort size 3: OK
sort size 64: OK
rsort size 3: OK
rsort size 64: OK
