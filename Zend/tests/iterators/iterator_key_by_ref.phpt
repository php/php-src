--TEST--
Iterator::key() with by-ref return
--FILE--
<?php
class Test extends ArrayIterator {
    #[ReturnTypeWillChange]
    function &key() {
        return $foo;
    }
}
foreach (new Test([0]) as $k => $v) {
    var_dump($k);
}
?>
--EXPECT--
NULL
