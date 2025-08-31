--TEST--
Tests that internal iterator's rewind function is called once
--EXTENSIONS--
zend_test
--FILE--
<?php

$subject = new \ZendTest\Iterators\TraversableTest();
$it = $subject->getIterator();
var_dump($it);
foreach ($it as $key => $value) {
    echo "{$key} => {$value}\n";
}
?>
--EXPECT--
object(InternalIterator)#3 (0) {
}
TraversableTest::rewind
TraversableTest::valid
TraversableTest::current
TraversableTest::key
0 => 0
TraversableTest::next
TraversableTest::valid
TraversableTest::current
TraversableTest::key
1 => 1
TraversableTest::next
TraversableTest::valid
TraversableTest::current
TraversableTest::key
2 => 2
TraversableTest::next
TraversableTest::valid
TraversableTest::current
TraversableTest::key
3 => 3
TraversableTest::next
TraversableTest::valid
TraversableTest::drop
