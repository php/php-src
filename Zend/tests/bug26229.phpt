--TEST--
Bug #26229 (getIterator() segfaults when it returns arrays or scalars)
--FILE--
<?php

class array_iterator implements IteratorAggregate {
        public function getIterator() {
                return array('foo', 'bar');     
        }
}

$obj = new array_iterator;

foreach ($obj as $property => $value) {
        var_dump($value);
}
?>
===DONE===
--EXPECTF--
Warning: Objects returned by array_iterator::getIterator() must be traversable or implement interface Iterator in %sbug26229.php on line %d
===DONE===