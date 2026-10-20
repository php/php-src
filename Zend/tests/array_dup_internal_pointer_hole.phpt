--TEST--
Array duplication relocates an internal pointer on a hole, with and without foreach iterators
--FILE--
<?php
function duplicate(array &$values): void {
    $copy = $values;
    $values['i'] = 18;
    echo 'current: ', key($values), '=>', current($values), "\n";
    next($values);
    echo 'next: ', key($values), '=>', current($values), "\n";
    echo 'copy current: ', key($copy), '=>', current($copy), "\n";
    echo 'copy keys: ', implode(' ', array_keys($copy)), "\n";
}

foreach ([false, true] as $withIterator) {
    echo $withIterator ? "With iterator:\n" : "Without iterator:\n";
    $values = ['a' => 10, 'b' => 11, 'c' => 12, 'd' => 13,
               'e' => 14, 'f' => 15, 'g' => 16, 'h' => 17];
    next($values);
    next($values);
    // Leave the internal pointer on a hole before several surviving elements.
    unset($values['a'], $values['b'], $values['c'], $values['d']);

    if ($withIterator) {
        foreach ($values as &$value) {
            duplicate($values);
            break;
        }
        unset($value);
    } else {
        duplicate($values);
    }
}
?>
--EXPECT--
Without iterator:
current: e=>14
next: f=>15
copy current: e=>14
copy keys: e f g h
With iterator:
current: e=>14
next: f=>15
copy current: e=>14
copy keys: e f g h
