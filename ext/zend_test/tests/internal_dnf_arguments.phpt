--TEST--
DNF types for internal functions
--EXTENSIONS--
zend_test
spl
reflection
--FILE--
<?php

$rf = new \ReflectionFunction('zend_test_internal_dnf_arguments');
var_dump((string)$rf->getReturnType());
$paramType = $rf->getParameters()[0]->getType();
var_dump((string)$paramType);

try {
    zend_test_internal_dnf_arguments(new stdClass);
} catch (\Throwable $err) {
    echo $err->getMessage(), "\n";
}

$obj = new \ArrayIterator([]);
$result = zend_test_internal_dnf_arguments($obj);
var_dump($result);

?>
--EXPECT--
string(32) "Iterator|(Traversable&Countable)"
string(32) "Iterator|(Traversable&Countable)"
zend_test_internal_dnf_arguments(): Argument #1 ($arg) must be of type Iterator|(Traversable&Countable), stdClass given
object(ArrayIterator)#5 (1) {
  ["storage":"ArrayIterator":private]=>
  array(0) {
  }
}
