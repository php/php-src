--TEST--
ArrayObject: an iterator keeps its numeric position when the storage is replaced
--FILE--
<?php
function position(string $source, array $unset) {
    $ao = new ArrayObject(['a' => 1, 'b' => 2, 'c' => 3, 'd' => 4]);
    $it = $ao->getIterator();
    $it->next();
    $it->next();
    switch ($source) {
        case 'copy':
            // The array is shared with the storage until unset() separates it,
            // then exchangeArray() shares it as is, holes included
            $array = $ao->getArrayCopy();
            foreach ($unset as $key) {
                unset($array[$key]);
            }
            $ao->exchangeArray($array);
            break;
        case 'packed':
            $ao->exchangeArray(array_diff_key(['a' => 1, 'b' => 2, 'c' => 3, 'd' => 4], array_flip($unset)));
            break;
        case 'holes':
            // An array with a refcount of 1 is used as is, holes included
            $ao->exchangeArray((function () use ($unset) {
                $array = ['a' => 1, 'b' => 2, 'c' => 3, 'd' => 4];
                foreach ($unset as $key) {
                    unset($array[$key]);
                }
                return $array;
            })());
            break;
    }
    echo $source, ': ';
    var_dump($it->valid() ? $it->key() : null);
}

foreach ([['a', 'b'], ['a']] as $unset) {
    position('copy', $unset);
    position('packed', $unset);
    position('holes', $unset);
}
?>
--EXPECT--
copy: string(1) "c"
packed: NULL
holes: string(1) "c"
copy: string(1) "c"
packed: string(1) "d"
holes: string(1) "c"
