--TEST--
SPL: Error: iterator_apply when an iterator method (eg rewind) throws exception
--FILE--
<?php

class MyArrayIterator extends ArrayIterator {
    public function rewind(): void {
        throw new Exception('Make the iterator break');
    }
}

function test() {}

$it = new MyArrayIterator(array(1, 21, 22));

try {
    $res = iterator_apply($it, 'test');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>

<?php exit(0); ?>
--EXPECT--
Exception: Make the iterator break
