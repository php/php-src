--TEST--
GH-14741 (Segmentation fault in Zend/zend_types.h)
--EXTENSIONS--
zend_test
--FILE--
<?php
$subject = new \ZendTest\Iterators\TraversableTest();
$it = $subject->getIterator();
try {
    clone $it;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Trying to clone an uncloneable object of class InternalIterator
TraversableTest::drop
