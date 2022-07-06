--TEST--
SPL: Error: iterator_to_array when the current operation throws an exception
--FILE--
<?php

class MyArrayIterator extends ArrayIterator {
    public function current() {
        throw new Exception('Make the iterator break');
    }
}

$it = new MyArrayIterator(array(4, 6, 2));

try {
    // get keys
    $ar = iterator_to_array($it);
} catch (Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    // get values
    $ar = iterator_to_array($it, false);
} catch (Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>

<?php exit(0); ?>
--EXPECT--
Make the iterator break
Make the iterator break
