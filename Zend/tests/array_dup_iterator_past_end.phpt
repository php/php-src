--TEST--
Array duplication preserves past-the-end iterators when compacting holes
--FILE--
<?php
$values = ['a' => 10, 'b' => 11, 'c' => 12, 'd' => 13];
unset($values['a'], $values['b']);

foreach ($values as $key => &$value) {
    echo "$key=>$value\n";
    if ($key === 'd') {
        // The iterator is one past the end; COW compacts the preceding holes.
        $copy = $values;
        $values['e'] = 14;
    }
}
unset($value);
echo 'copy: ', implode(' ', array_keys($copy)), "\n";
?>
--EXPECT--
c=>12
d=>13
e=>14
copy: c d
