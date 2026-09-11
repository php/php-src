--TEST--
Packed zval sorting handles recursive array references and collection after sorting
--FILE--
<?php
foreach (['sort', 'rsort'] as $sort) {
    $values = range(64, 1);
    $values[0] = &$values;
    $values[1] = &$values;
    $sort($values, SORT_REGULAR);
    var_dump(count($values), array_is_list($values));
    $index = $sort === 'sort' ? 62 : 0;
    var_dump($values[$index] === $values, $values[$index + 1] === $values);
    unset($values);
    gc_collect_cycles();

    $values = range(64, 1);
    $left = [&$values];
    $right = [&$left];
    $values[0] = &$left;
    $values[1] = &$right;
    $sort($values, SORT_REGULAR);
    var_dump(count($values), array_is_list($values));
    unset($values, $left, $right);
    gc_collect_cycles();
}
?>
--EXPECT--
int(64)
bool(true)
bool(true)
bool(true)
int(64)
bool(true)
int(64)
bool(true)
bool(true)
bool(true)
int(64)
bool(true)
