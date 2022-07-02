--TEST--
Bug #75173 incorrect behavior of AppendIterator::append in foreach loop
--FILE--
<?php

$it = new \AppendIterator();
$it->append(new ArrayIterator(['foo']));

foreach ($it as $item) {
    var_dump($item);

    if ('foo' === $item) {
        $it->append(new ArrayIterator(['bar']));
    }
}
?>
--EXPECT--
string(3) "foo"
string(3) "bar"	
